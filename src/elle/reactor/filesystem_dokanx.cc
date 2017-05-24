#define _FILE_OFFSET_BITS 64
#include <locale>
#include <thread>
#include <fcntl.h>
#include <sys/stat.h>

#undef stat

#include <elle/reactor/filesystem.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/reactor/Barrier.hh>

#include <elle/windows/string_conversion.hh>
#include <elle/os/environ.hh>
#include <elle/finally.hh>
#include <ntstatus.h>
using NTSTATUS = DWORD;

#include <dokan/dokan.h>
#include <strsafe.h>
#undef wcscpy

ELLE_LOG_COMPONENT("elle.reactor.filesystem.dokanx");

namespace elle
{
  namespace reactor
  {
    namespace filesystem
    {
      namespace
      {
        Scheduler* le_scheduler;
        // Set to true if callbacks are called in the reactor thread.
        bool dokan_sync = false;

        NTSTATUS to_ntstatus(int erc)
        {
          switch(erc)
          {
          case ENOENT:
            return STATUS_OBJECT_NAME_NOT_FOUND;
          case EPERM:
          case EACCES:
            return STATUS_ACCESS_DENIED;
          case ENOSYS:
            return STATUS_NOT_IMPLEMENTED;
          case EEXIST:
            return STATUS_OBJECT_NAME_EXISTS;
          case EISDIR:
            return STATUS_FILE_IS_A_DIRECTORY;
          case ENOTDIR:
            return STATUS_NOT_A_DIRECTORY;
          case ENODATA:
            return STATUS_OBJECT_NAME_NOT_FOUND;
          case EINVAL:
            return STATUS_INVALID_PARAMETER;
          case ENOSPC:
            return STATUS_ALLOTTED_SPACE_EXCEEDED;
          default:
            return STATUS_NOT_IMPLEMENTED;
          }
        }

        std::wstring from_utf8(std::string const& source)
        {
          using facet_type = std::codecvt<wchar_t,char,std::mbstate_t>;
          std::locale mylocale;
          const facet_type& myfacet = std::use_facet<facet_type>(mylocale);

          // prepare objects to be filled by codecvt::in :
          wchar_t pwstr[source.length()+1];              // the destination buffer (might be too short)
          std::mbstate_t mystate = std::mbstate_t(); // the shift state object
          const char* pc;                            // from_next
          wchar_t* pwc;                              // to_next

          // translate characters:
          facet_type::result myresult = myfacet.in (mystate,
            source.data(), source.data()+source.length(), pc,
            pwstr, pwstr+source.length()+1, pwc);
          if (myresult != facet_type::ok)
            throw Error(0, "transcoding failed");
          return std::wstring(pwstr);
        }

        std::string to_utf8(LPCWSTR pszTextUTF16)
        {

          //
          // Special case of NULL or empty input string
          //
          if ( (pszTextUTF16 == NULL) || (*pszTextUTF16 == 0) )
          {
            // Return empty string
            return "";
          }
          //
          // Consider WCHAR’s count corresponding to total input string length,
          // including end-of-string (L) character.
          //
          size_t cchUTF16Max = 10000 - 1;
          size_t cchUTF16;
          if (FAILED(::StringCchLengthW( pszTextUTF16, cchUTF16Max, &cchUTF16)))
            ELLE_DEBUG("StringCchLengthW failed");
          // Consider also terminating
          ++cchUTF16;
          //
          // WC_ERR_INVALID_CHARS flag is set to fail if invalid input character
          // is encountered.
          // This flag is supported on Windows Vista and later.
          // Don’t use it on Windows XP and previous.
          //
          #if (WINVER >= 0x0600)
          DWORD dwConversionFlags = WC_ERR_INVALID_CHARS;
          #else
          DWORD dwConversionFlags = 0;
          #endif

          //
          // Get size of destination UTF-8 buffer, in CHAR’s (= bytes)
          //
          int cbUTF8 = ::WideCharToMultiByte(
            CP_UTF8,                // convert to UTF-8
            dwConversionFlags,      // specify conversion behavior
            pszTextUTF16,           // source UTF-16 string
            static_cast<int>( cchUTF16 ),   // total source string length, in WCHAR’s,
                                            // including end-of-string
            NULL,                   // unused – no conversion required in this step
            0,                      // request buffer size
            NULL, NULL              // unused
            );


          //
          // Allocate destination buffer for UTF-8 string
          //
          int cchUTF8 = cbUTF8; // sizeof(CHAR) = 1 byte
          std::string strUTF8(cchUTF8, '\0');

          char * pszUTF8 = (char*)strUTF8.data();
          //
          // Do the conversion from UTF-16 to UTF-8
          //
          if (::WideCharToMultiByte(
              CP_UTF8,                // convert to UTF-8
              dwConversionFlags,      // specify conversion behavior
              pszTextUTF16,           // source UTF-16 string
              static_cast<int>( cchUTF16 ),   // total source string length, in WCHAR’s,
                                              // including end-of-string
              pszUTF8,                // destination buffer
              cbUTF8,                 // destination buffer size, in bytes
              NULL, NULL              // unused
            ))
            ELLE_LOG("WideCharToMultiByte failed: %s", GetLastError())

          // Return resulting UTF-8 string
          strUTF8 = strUTF8.substr(0, strUTF8.size()-1);
          return strUTF8;
        }

        __attribute__((__stdcall__))
        NTSTATUS CreateFile(
            LPCWSTR fileName,
            PDOKAN_IO_SECURITY_CONTEXT sctx,
            DWORD  desiredAccess,
            DWORD fileAttributes,
            DWORD  shareMode,
            DWORD  creationDisposition,
            DWORD  createOptions,
            PDOKAN_FILE_INFO context)
        {
          ELLE_TRACE("createfile");
          context->Context = 0;
          auto work = [&]() -> NTSTATUS {
          try
          {
            FileSystem* fs = (FileSystem*)context->DokanOptions->GlobalContext;
            std::string path = to_utf8(fileName);
            std::shared_ptr<Path> p;
            try
            {
              p = fs->path(path);
            }
            catch (Error const& e)
            {
              if (e.error_code() == ENOENT)
                return STATUS_OBJECT_NAME_NOT_FOUND;
              else if (e.error_code() == EPERM)
                return STATUS_ACCESS_DENIED;
              else
              {
                ELLE_TRACE("Unhandled specific code %s", e.error_code());
                return STATUS_OBJECT_NAME_NOT_FOUND;
              }
            }
            mode_t mode = 0;
            if ((desiredAccess & GENERIC_READ) && (desiredAccess & GENERIC_WRITE))
              mode = O_RDWR;
            else if (desiredAccess & GENERIC_READ)
              mode = O_RDONLY;
            else
              mode = O_WRONLY;
            if (creationDisposition == FILE_SUPERSEDE
              || creationDisposition == FILE_OVERWRITE_IF)
              mode = O_CREAT | O_TRUNC;
            if (creationDisposition == FILE_CREATE)
              mode = O_CREAT | O_EXCL;
            if (creationDisposition == FILE_OPEN_IF)
              mode = O_CREAT;
            if (creationDisposition == FILE_OPEN)
              mode = 0;
            if (creationDisposition == FILE_OVERWRITE)
              mode = O_TRUNC;
            ELLE_TRACE("createFile %s, cd=%s, trunc=%s, creat=%s",
                       path, creationDisposition,
                       !!(mode & O_TRUNC), !!(mode & O_CREAT));
            int access = 0644;
            bool exists = false;
            struct stat st;
            memset(&st, 0, sizeof(struct stat));
            try
            {
              p->stat(&st);
              exists = true;
            }
            catch (Error const& e)
            {
              ELLE_DEBUG("stat failed: %s", e);
            }
            ELLE_DEBUG("stat, mode=%s", st.st_mode);
            if (exists && !(st.st_mode & 0777))
              return STATUS_ACCESS_DENIED;
            if (!exists && !(mode & O_CREAT))
            {
              ELLE_DEBUG("entry does not exists");
              context->Context = 0;
              return 0xC0000034; // STATUS_OBJECT_NAME_NOT_FOUND
              //return 0xC000000F; // STATUS_NO_SUCH_FILE
            }
            if (exists && (mode & O_EXCL))
            {
              ELLE_DEBUG("entry already exists");
              context->Context = 0;
              return 0xC0000035; //STATUS_OBJECT_NAME_COLLISION
              // or is it return 0x40000000; //STATUS_OBJECT_NAME_EXISTS
            }
            if (exists && S_ISDIR(st.st_mode))
            {
              ELLE_DEBUG("directory, nothing to do");
              context->IsDirectory = TRUE;
              context->Context = 0;
              return 0;
            }
            if (exists && (mode & O_CREAT))
            {
              SetLastError(ERROR_ALREADY_EXISTS);
            }
            if (createOptions & FILE_DIRECTORY_FILE)
            {
              try
              {
                struct stat st;
                p->stat(&st);
              }
              catch (Error const& e)
              {
                p->mkdir(0755);
              }
              context->IsDirectory = TRUE;
              context->Context = 0;
            }
            else
            {
              auto handle = p->create(mode, access);
              context->Context = (decltype(context->Context))handle.release();
            }
          }
          catch (Error const& e)
          {
            ELLE_TRACE("Filesystem error creating: %s", e);
            return to_ntstatus(e.error_code());
          }
          return 0;
          };
          if (dokan_sync)
            return work();
          else
            return le_scheduler->mt_run<NTSTATUS>("createfile", work);
        }

        __attribute__((__stdcall__))
        void Cleanup(LPCWSTR fileName, PDOKAN_FILE_INFO context)
        {
          ELLE_TRACE("cleanup");
          auto work =  [&] {
          try
          {
            FileSystem* fs = (FileSystem*)context->DokanOptions->GlobalContext;
            std::string path = to_utf8(fileName);
            ELLE_TRACE("cleanup %s, doc=%s", path, !!context->DeleteOnClose);
            if (context->DeleteOnClose)
            {
              auto p = fs->path(path);
              if (context->IsDirectory)
                p->rmdir();
              else
                p->unlink();
            }
            Handle* handle = (Handle*)context->Context;
            if (handle)
            {
              ELLE_DEBUG("Closing handle %s", (void*)handle);
              handle->close();
              ELLE_DEBUG("Deleting handle %s", (void*)handle);
              delete handle;
              context->Context = 0;
              ELLE_DEBUG("Done");
            }
          }
          catch (Error const& e)
          {
            ELLE_WARN("Filesystem error cleaning up: %s", e);
          }
          };
          if (dokan_sync)
            work();
          else
            le_scheduler->mt_run<void>("cleanup", work);
        }

        __attribute__((__stdcall__))
        void CloseFile(LPCWSTR fileName, PDOKAN_FILE_INFO context)
        {
          ELLE_TRACE("closefile");
          auto work = [&] {
          std::string path = to_utf8(fileName);
          Handle* handle = (Handle*)context->Context;
          ELLE_DEBUG("closefile %s, h=%x", path, handle);
          if (handle)
          {
            handle->close();
            delete handle;
            context->Context = 0;
          }};
          if (dokan_sync)
            work();
          else
            le_scheduler->mt_run<void>("closefile", work);
        }

        __attribute__((__stdcall__))
        NTSTATUS ReadFile(LPCWSTR fileName,
                                 void*    Buffer,
                                 DWORD    NumberOfBytesToRead,
                                 LPDWORD  NumberOfBytesRead,
                                 LONGLONG Offset,
                                 PDOKAN_FILE_INFO context)
        {
          auto work = [&]()->NTSTATUS {
          try
          {
            bool temp_handle = false;
            //FileSystem* fs = (FileSystem*)context->DokanOptions->GlobalContext;
            //std::string path = to_utf8(fileName);
            //Path& p = fs->path(path);
            std::string path = to_utf8(fileName);
            ELLE_TRACE("readfile %s : %s  bytes from %s",
                       fileName, NumberOfBytesToRead, Offset);
            if (Offset < 0)
              Offset = 0;
            Handle* handle = (Handle*)context->Context;
            if (!handle)
            {
              ELLE_WARN("readfile on %s, no handle", path);
              // Possibly a dokan bug, but this happens when previewing images
              // under the windows explorer. Traces show a cleanup just before
              // the read
              bool prev_sync = dokan_sync;
              dokan_sync = true;
              elle::SafeFinally reset([&] { dokan_sync = prev_sync;});
              CreateFile(fileName, NULL, GENERIC_READ, 0, 0, FILE_OPEN, 0, context);
              handle = (Handle*)context->Context;
              if (!handle)
              {
                ELLE_WARN("CreateFile workaround failed");
                return 0xC0000185; // STATUS_IO_DEVICE_ERROR
              }
              temp_handle = true;
            }
            DWORD res = 0;
            if (NumberOfBytesToRead > 512 * 1024)
            {
              // split it
              unsigned char* buf = (unsigned char*)Buffer;
              while (true)
              {
                auto to_read = std::min(NumberOfBytesToRead, (DWORD)(512 * 1024));
                int r = handle->read(elle::WeakBuffer(buf + res, to_read),
                                      to_read, Offset + res);
                res += r;
                NumberOfBytesToRead -= r;
                if ((unsigned)r < to_read || !NumberOfBytesToRead)
                  break;
              }
            }
            else
              res = handle->read(elle::WeakBuffer(Buffer, NumberOfBytesToRead),
                                 NumberOfBytesToRead, Offset);
            if (NumberOfBytesRead)
              *NumberOfBytesRead = res;
            if (temp_handle)
            {
              handle->close();
              delete handle;
              context->Context = 0;
            }
            return 0;
          }
          catch (Error const& e)
          {
            ELLE_TRACE("Filesystem error reading: %s", e);
            return to_ntstatus(e.error_code());
          }};
          if (dokan_sync)
            return work();
          else
            return le_scheduler->mt_run<NTSTATUS>("readfile", work);
        }

        __attribute__((__stdcall__))
        NTSTATUS WriteFile(LPCWSTR fileName,
                                 const void*   Buffer,
                                 DWORD    NumberOfBytesToRead,
                                 LPDWORD  NumberOfBytesRead,
                                 LONGLONG Offset,
                                 PDOKAN_FILE_INFO context)
        {
          auto work = [&]() -> NTSTATUS{
          try
          {
            FileSystem* fs = (FileSystem*)context->DokanOptions->GlobalContext;
            std::string path = to_utf8(fileName);
            ELLE_TRACE("writefile %s : %s  bytes from %s",
                       fileName, NumberOfBytesToRead, Offset);
            if (Offset == -1)
            {
              auto p = fs->path(path);
              struct stat st;
              p->stat(&st);
              Offset = st.st_size;
            }
            //Path& p = fs->path(path);
            Handle* handle = (Handle*)context->Context;
            int res = handle->write(elle::WeakBuffer((void*)Buffer, NumberOfBytesToRead),
                                   NumberOfBytesToRead, Offset);
            if (NumberOfBytesRead)
              *NumberOfBytesRead = res;
            return 0;
          }
          catch (Error const& e)
          {
            ELLE_TRACE("Filesystem error writing  %s", e);
            return to_ntstatus(e.error_code());
          }};
          if (dokan_sync)
            return work();
          else
            return le_scheduler->mt_run<NTSTATUS>("writefile", work);
        }

        FILETIME to_filetime(time_t t)
        { // time_t time as the number of seconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).
          // filetime: Contains a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC)
          // seriously? FIXME: compute the offset once and for all
          uint64_t fnow = (uint64_t)t * 10000000ULL;
          fnow += 116444736000000000ULL;
          return FILETIME{fnow & 0xFFFFFFFF, fnow >> 32};
        }

        uint64_t from_filetime(FILETIME const& ft)
        {
          uint64_t fnow = ft.dwLowDateTime + ((uint64_t)ft.dwHighDateTime << 32);
          fnow -= 116444736000000000ULL;
          fnow /= 10000000ULL;
          return fnow;
        }

        __attribute__((__stdcall__))
        NTSTATUS GetFileInformation(LPCWSTR fileName,
                                           LPBY_HANDLE_FILE_INFORMATION buffer,
                                           PDOKAN_FILE_INFO context)
        {
          auto work = [&]() -> NTSTATUS {
          try
          {
            FileSystem* fs = (FileSystem*)context->DokanOptions->GlobalContext;
            std::string path = to_utf8(fileName);
            auto p = fs->path(path);
            struct stat st;
            p->stat(&st);
            if (st.st_mode & 0040000)
              buffer->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
            else
              buffer->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
            ELLE_TRACE("stat(%s): size %s, mtime %s", sizeof(st), st.st_size, st.st_mtime);
            buffer->ftCreationTime = to_filetime(st.st_ctime);
            buffer->ftLastWriteTime = to_filetime(st.st_mtime);
            buffer->ftLastAccessTime = to_filetime(st.st_atime);
            buffer->nFileSizeHigh = 0; // FIXME BROKEN st.st_size >> 32;
            buffer->nFileSizeLow = st.st_size & 0xFFFFFFFF;
            buffer->nNumberOfLinks = st.st_nlink;
            return 0;
          }
          catch (Error const& e)
          {
            ELLE_TRACE("Filesystem error GetFileInformation: %s", e);
            return to_ntstatus(e.error_code());
          }
          return 0;
          };
          if (dokan_sync)
            return work();
          else
            return le_scheduler->mt_run<NTSTATUS>("getinfo", work);
        }

        __attribute__((__stdcall__))
        NTSTATUS FindFiles(LPCWSTR fileName, PFillFindData cb,
                                  PDOKAN_FILE_INFO context)
        {
          std::vector<WIN32_FIND_DATAW> res;
          auto work = [&] () -> NTSTATUS
          {
            try
            {
              FileSystem* fs = (FileSystem*)context->DokanOptions->GlobalContext;
              std::string parent_path = to_utf8(fileName);
              auto p = fs->path(parent_path);
              std::vector<std::string> hits;
              p->list_directory([&](std::string const&path, struct stat* st) {
                  hits.push_back(path);
              });
              ELLE_DEBUG("got %s hits on %s", hits.size(), parent_path);
              for (auto const& path: hits)
              {
                if (path == "." || path == "..")
                  continue; // we handle those ourselve
                ELLE_DEBUG("scanning entry %s/%s (%s)", parent_path, path, sizeof(struct stat));
                int g1 = 0xfefefefe;
                struct stat lst;
                int g2 = 0xefefefef;
                // Always restat, list_directory stat info is bogus
                fs->path((bfs::path(parent_path) / path).string())->stat(&lst);
                ELLE_ASSERT_EQ(g1, (int)0xfefefefe);
                ELLE_ASSERT_EQ(g2, (int)0xefefefef);
                WIN32_FIND_DATAW fd;
                memset(&fd, 0, sizeof(fd));
                if (lst.st_mode & 0040000)
                  fd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
                else
                  fd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
                fd.ftCreationTime = to_filetime(lst.st_ctime);
                fd.ftLastWriteTime = to_filetime(lst.st_mtime);
                fd.ftLastAccessTime = to_filetime(lst.st_atime);
                // FIXME BROKEN fd.nFileSizeHigh = lst.st_size >> 32;
                fd.nFileSizeLow = lst.st_size & 0xFFFFFFFF;
                std::string src = bfs::path(path).filename().string();
                ELLE_TRACE("converting %s", src);
                ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                  src.data(), src.size(), fd.cFileName, MAX_PATH);
                res.push_back(fd);
              }
              // . and ..
              struct stat st;
              p->stat(&st);
              WIN32_FIND_DATAW fd;
              memset(&fd, 0, sizeof(fd));
              fd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
              fd.ftCreationTime = to_filetime(st.st_ctime);
              fd.ftLastWriteTime = to_filetime(st.st_mtime);
              fd.ftLastAccessTime = to_filetime(st.st_atime);
              memcpy(fd.cFileName, ".", 1);
              res.push_back(fd);
              memcpy(fd.cFileName, ".\0.\0", 4);
              res.push_back(fd);
            }
            catch (Error const& e)
            {
              ELLE_TRACE("Filesystem error FindFileing: %s", e);
              return to_ntstatus(e.error_code());
            }
            return 0;
          };
          NTSTATUS st;
          if (dokan_sync)
            st = work();
          else
            st = le_scheduler->mt_run<NTSTATUS>("findfile", work);
          if (st)
            return st;
          for (auto& r: res)
            cb(&r, context);
          return 0;
        }

        __attribute__((__stdcall__))
        NTSTATUS DeleteFile(LPCWSTR fileName, PDOKAN_FILE_INFO context)
        { // see doc, action happens elsewhere
          return 0;
        }

        __attribute__((__stdcall__))
        NTSTATUS DeleteDirectory(LPCWSTR fileName, PDOKAN_FILE_INFO context)
        { // see doc, action happens elsewhere
          ELLE_TRACE("DeleteDirectory");
          auto work = [&]() -> NTSTATUS {
            try
            {
              FileSystem* fs = (FileSystem*)context->DokanOptions->GlobalContext;
              std::string path = to_utf8(fileName);
              auto p = fs->path(path);
              int count = 0;
              p->list_directory([&](std::string const&, struct stat*)
                {
                  ++count;
                });
              if (count)
                return 0xC0000101; // STATUS_DIRECTORY_NOT_EMPTY;
              else
                return 0;
            }
            catch (Error const& e)
            {
              ELLE_TRACE("Filesystem error deleting  %s", e);
              return to_ntstatus(e.error_code());
            }
          };
          if (dokan_sync)
            return work();
          else
            return le_scheduler->mt_run<NTSTATUS>("deletedirectory", work);
        }

        __attribute__((__stdcall__))
        NTSTATUS FlushFileBuffers(LPCWSTR fileName, PDOKAN_FILE_INFO context)
        {
          return 0;
        }

        __attribute__((__stdcall__))
        NTSTATUS SetFileAttributes(LPCWSTR fileName, DWORD attributes,
                                          PDOKAN_FILE_INFO context)
        {
          return 0;
        }

        __attribute__((__stdcall__))
        NTSTATUS SetFileTime(LPCWSTR fileName,
                                    CONST FILETIME* creationTime,
                                    CONST FILETIME* lastAccessTime,
                                    CONST FILETIME*  lastWriteTime,
                                    PDOKAN_FILE_INFO context
                                    )
        {
          ELLE_TRACE("SetFileTime");
          auto work = [&]() -> NTSTATUS {
            try
            {
              FileSystem* fs = (FileSystem*)context->DokanOptions->GlobalContext;
              std::string path = to_utf8(fileName);
              auto p = fs->path(path);
              struct timespec tv[2] = {{0,0},{0,0}};
              if (lastAccessTime)
                tv[0].tv_sec = from_filetime(*lastAccessTime);
              if (lastWriteTime)
                tv[1].tv_sec = from_filetime(*lastWriteTime);
              p->utimens(tv);
            }
            catch (Error const& e)
            {
              ELLE_TRACE("Filesystem error setfiletiming %s", e);
              return to_ntstatus(e.error_code());
            }
            return 0;
          };
          if (dokan_sync)
            return work();
          else
            return le_scheduler->mt_run<NTSTATUS>("setfiletime", work);
        }

        __attribute__((__stdcall__))
        NTSTATUS MoveFile(LPCWSTR fileName, LPCWSTR wtarget,
                                 BOOL replaceExisting,
                                 PDOKAN_FILE_INFO context)
        {
          auto work = [&]()->NTSTATUS {
          try
          {
            FileSystem* fs = (FileSystem*)context->DokanOptions->GlobalContext;
            std::string path = to_utf8(fileName);
            auto p = fs->path(path);
            std::string target = to_utf8(wtarget);
            if (!replaceExisting)
            {
              try
              {
                struct stat st;
                fs->path(target)->stat(&st);
                return STATUS_ACCESS_DENIED;
              }
              catch(...)
              {}
            }
            p->rename(target);
          }
          catch (Error const& e)
          {
            ELLE_TRACE("Filesystem error moving : %s", e);
            return to_ntstatus(e.error_code());
          }
          return 0;
          };
          if (dokan_sync)
            return work();
          else
            return le_scheduler->mt_run<NTSTATUS>("movefile", work);
        }

        __attribute__((__stdcall__))
        NTSTATUS SetEndOfFile(LPCWSTR fileName,
                                     LONGLONG length,
                                     PDOKAN_FILE_INFO context)
        {
          auto work = [&]()->NTSTATUS {
          try
          {
            FileSystem* fs = (FileSystem*)context->DokanOptions->GlobalContext;
            std::string path = to_utf8(fileName);
            auto p = fs->path(path);
            p->truncate(length);
          }
          catch (Error const& e)
          {
            ELLE_TRACE("Filesystem error truncating: %s", e);
            return to_ntstatus(e.error_code());
          }
          return 0;
          };
          if (dokan_sync)
            return work();
          else
            return le_scheduler->mt_run<NTSTATUS>("truncate", work);
        }

        __attribute__((__stdcall__))
        NTSTATUS SetAllocationSize(LPCWSTR fileName,
                                          LONGLONG length,
                                          PDOKAN_FILE_INFO context)
        {
          auto work = [&]()->NTSTATUS {
          try
          {
            FileSystem* fs = (FileSystem*)context->DokanOptions->GlobalContext;
            std::string path = to_utf8(fileName);
            auto p = fs->path(path);
            struct stat st;
            p->stat(&st);
            if (st.st_size > length)
              p->truncate(length);
          }
          catch (Error const& e)
          {
            ELLE_TRACE("Filesystem error setting alloc size: %s", e);
            return to_ntstatus(e.error_code());
          }
          return 0;
          };
          if (dokan_sync)
            return work();
          else
            return le_scheduler->mt_run<NTSTATUS>("allocsize", work);
        }

        __attribute__((__stdcall__))
        NTSTATUS LockFile( LPCWSTR, // FileName
                                 LONGLONG, // ByteOffset
                                 LONGLONG, // Length
                                 PDOKAN_FILE_INFO)
        {
          return STATUS_ACCESS_DENIED;
        }

        __attribute__((__stdcall__))
        NTSTATUS UnlockFile( LPCWSTR, // FileName
                                 LONGLONG, // ByteOffset
                                 LONGLONG, // Length
                                 PDOKAN_FILE_INFO)
        {
          return STATUS_ACCESS_DENIED;
        }

        __attribute__((__stdcall__))
        NTSTATUS GetDiskFreeSpace(PULONGLONG freeBytes,
                                         PULONGLONG totalBytes,
                                         PULONGLONG totalFreeBytes,
                                         PDOKAN_FILE_INFO context)
        {
          auto work = [&]() -> NTSTATUS {
          try
          {
            FileSystem* fs = (FileSystem*)context->DokanOptions->GlobalContext;
            std::string path = "/";
            auto p = fs->path(path);
            struct statvfs s;
            p->statfs(&s);
            if (totalBytes)
              *totalBytes = (ULONGLONG)s.f_frsize * s.f_blocks;
            if (freeBytes)
              *freeBytes = (ULONGLONG)s.f_frsize * s.f_bfree;
            if (totalFreeBytes)
              *totalFreeBytes = *freeBytes; //FIXME what is this?
          }
          catch (Error const& e)
          {
            ELLE_TRACE("Filesystem error getting free space: %s", e);
            return to_ntstatus(e.error_code());
          }
          return 0;
          };
          if (dokan_sync)
            return work();
          else
            return le_scheduler->mt_run<NTSTATUS>("freespace", work);
        }

        __attribute__((__stdcall__))
        NTSTATUS GetVolumeInformation(LPWSTR VolumeNameBuffer,
                                             DWORD  VolumeNameSize,
                                             LPDWORD VolumeSerialNumber,
                                             LPDWORD MaximumComponentLength,
                                             LPDWORD FileSystemFlags,
                                             LPWSTR  FileSystemNameBuffer,
                                             DWORD        FileSystemNameSize,
                                             PDOKAN_FILE_INFO context)
        {
          FileSystem* fs = (FileSystem*)context->DokanOptions->GlobalContext;
          if (VolumeNameBuffer)
          {
            wcscpy(VolumeNameBuffer, L"infinit");
            for (auto o: fs->mount_options())
            {
              if (o.find("volname=") == 0)
              {
                auto volname = o.substr(strlen("volname="));
                auto volnamew = from_utf8(volname);
                wcscpy(VolumeNameBuffer, volnamew.data());
              }
            }
          }
          if (VolumeSerialNumber)
            *VolumeSerialNumber = 0x00000001;
          if (FileSystemNameBuffer)
            wcscpy(FileSystemNameBuffer, L"dokan");
          if (MaximumComponentLength)
            *MaximumComponentLength = 256;
          if (FileSystemFlags)
            *FileSystemFlags = FILE_CASE_SENSITIVE_SEARCH |
                              FILE_CASE_PRESERVED_NAMES |
                            FILE_SUPPORTS_HARD_LINKS |
                            FILE_UNICODE_ON_DISK;
          return 0;
        }

        __attribute__((__stdcall__))
        NTSTATUS Unmount(PDOKAN_FILE_INFO	context)
        {
          ELLE_LOG("Filesystem umount notification received.");
          return STATUS_SUCCESS;
        }

      }

      class FileSystemImpl
      {
      public:
        std::unique_ptr<std::thread> _thread;
        std::unique_ptr<reactor::Thread> _rThread;
        reactor::Barrier _barrier;
        DOKAN_OPERATIONS* dokanOperations;
        DOKAN_OPTIONS* dokanOptions;
      };

      namespace
      {
        void dokan_run(void* arg)
        {
          auto fs = (FileSystem*)arg;
          int status = DokanMain(fs->impl()->dokanOptions, fs->impl()->dokanOperations);
          fs->impl()->_barrier.open();
          switch(status)
          {
          case DOKAN_SUCCESS:
            fprintf(stderr, "Success");
            break;
          case DOKAN_ERROR:
            fprintf(stderr, "Error");
            break;
          case DOKAN_DRIVE_LETTER_ERROR:
            fprintf(stderr, "Bad Drive letter");
            break;
          case DOKAN_DRIVER_INSTALL_ERROR:
            fprintf(stderr, "Can't install driver");
            break;
          case DOKAN_START_ERROR:
            fprintf(stderr, "Driver something wrong");
            break;
          case DOKAN_MOUNT_ERROR:
            fprintf(stderr, "Can't assign a drive letter");
            break;
          case DOKAN_MOUNT_POINT_ERROR:
            fprintf(stderr, "Mount point error");
            break;
          default:
            fprintf(stderr, "Unknown error: %d", status);
            break;
          }
        }
      }

      FileSystem::FileSystem(std::unique_ptr<Operations> op, bool full_tree)
        : _impl(new FileSystemImpl())
        , _operations(std::move(op))
        , _full_tree(full_tree)
      {
        this->_operations->filesystem(this);
      }

      void FileSystem::unmount()
      {
        // Without this flush, we get a strange trailing ",'" on stdout when
        // closing stdin of `infinit-volume --run --script` which breaks the
        // JSON output (tests/functional/passport-restrictions notably). This
        // might be wine only.
        std::cout.flush();
        std::wstring w = from_utf8(this->_where);
        DokanUnmount(w[0]);
      }

      void
      FileSystem::kill()
      {
        // FIXME
        ELLE_ERR("terminate not yet implemented for Dokan");
      }

      void FileSystem::mount(bfs::path const& where,
                             std::vector<std::string> const& options)
      {
        this->_mount_options = options;
        le_scheduler = &scheduler();
        DOKAN_OPTIONS* dokanOptions = new DOKAN_OPTIONS;
        memset(dokanOptions, 0, sizeof(DOKAN_OPTIONS));
        dokanOptions->Version = DOKAN_VERSION;
        dokanOptions->ThreadCount = 1; // use default
        dokanOptions->Timeout = 300000; // irp timeout in ms
        auto& logger = elle::log::logger();
        if (logger.component_is_active("reactor.filesystem.dokanx",
                                       elle::log::Logger::Level::debug))
        {
          dokanOptions->Options |= DOKAN_OPTION_DEBUG;
          dokanOptions->Options |= DOKAN_OPTION_STDERR;
        }
        std::string mode = elle::os::getenv("DOKAN_MOUNT_MODE", "");
        if (mode.empty())
          dokanOptions->Options |= DOKAN_OPTION_REMOVABLE;
        else
        {
          if (mode.find("removable")!=mode.npos)
            dokanOptions->Options |= DOKAN_OPTION_REMOVABLE;
          if (mode.find("network")!=mode.npos)
            dokanOptions->Options |= DOKAN_OPTION_NETWORK;
          if (mode.find("manager")!=mode.npos)
            dokanOptions->Options |= DOKAN_OPTION_MOUNT_MANAGER;
          if (mode.find("alt")!=mode.npos)
            dokanOptions->Options |= DOKAN_OPTION_ALT_STREAM;
          if (mode.find("session")!=mode.npos)
            dokanOptions->Options |= DOKAN_OPTION_CURRENT_SESSION;
        }
        for (auto o: options)
          if (o == "ro")
            dokanOptions->Options |= DOKAN_OPTION_WRITE_PROTECT;
        dokanOptions->GlobalContext = (decltype(dokanOptions->GlobalContext))this;
        std::wstring* mountPoint = new std::wstring(from_utf8(where.string()));
        dokanOptions->MountPoint = mountPoint->c_str();
        DOKAN_OPERATIONS* dokanOperations = new DOKAN_OPERATIONS;
        memset(dokanOperations , 0, sizeof(DOKAN_OPERATIONS));
        dokanOperations->ZwCreateFile = CreateFile;
        dokanOperations->Cleanup = Cleanup;
        dokanOperations->CloseFile = CloseFile;
        dokanOperations->ReadFile = ReadFile;
        dokanOperations->WriteFile = WriteFile;
        dokanOperations->FlushFileBuffers = FlushFileBuffers;
        dokanOperations->GetFileInformation = GetFileInformation;
        dokanOperations->FindFiles = FindFiles;
        dokanOperations->FindFilesWithPattern = nullptr;
        dokanOperations->SetFileAttributes = SetFileAttributes;
        dokanOperations->SetFileTime = SetFileTime;
        dokanOperations->DeleteFile = DeleteFile;
        dokanOperations->DeleteDirectory = DeleteDirectory;
        dokanOperations->MoveFile = MoveFile;
        dokanOperations->SetEndOfFile = SetEndOfFile;
        dokanOperations->SetAllocationSize = SetAllocationSize;
        dokanOperations->LockFile = LockFile;
        dokanOperations->UnlockFile = UnlockFile;
        dokanOperations->GetFileSecurity = nullptr;
        dokanOperations->SetFileSecurity = nullptr;
        dokanOperations->GetDiskFreeSpace = GetDiskFreeSpace;
        dokanOperations->GetVolumeInformation = GetVolumeInformation;
        dokanOperations->Unmounted = Unmount;
        _impl->dokanOperations = dokanOperations;
        _impl->dokanOptions = dokanOptions;
        if (dokan_sync)
          dokan_run(this);
        else
        {
          new std::thread([&] { dokan_run(this);});
          // _beginthread(dokan_run, 1024 * 1024, this);
          reactor::wait(_impl->_barrier);
        }
      }

      FileSystem::~FileSystem()
      {
        unmount();
        delete _impl;
      }
    }
  }
}
