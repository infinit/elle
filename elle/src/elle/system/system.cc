#ifndef INFINIT_WINDOWS
  #define _LARGEFILE64_SOURCE
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
  #include <fcntl.h>
#endif

#include <elle/log.hh>
#include <elle/finally.hh>
#include <elle/system/system.hh>
#include <elle/os/locale.hh>

#include <boost/system/error_code.hpp>
#include <boost/filesystem/fstream.hpp>

ELLE_LOG_COMPONENT("elle.system");

namespace elle
{
  namespace system
  {
    void truncate(boost::filesystem::path path,
                  uint64_t size)
    {
      using namespace boost::system::errc; // make_error_code and errors
#ifdef INFINIT_WINDOWS
      HANDLE h = CreateFileW(
        std::wstring(path.string().begin(), path.string().end()).c_str(),
        GENERIC_WRITE,
        0, 0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);
      if (h == INVALID_HANDLE_VALUE)
      {
        throw boost::filesystem::filesystem_error(
          "CreateFile",
          path,
          boost::system::error_code(::GetLastError(),
                                    boost::system::system_category()));
      }
      LONG offsetHigh = size >> 32;
      SetFilePointer(h,
                     static_cast<DWORD>(size),
                     &offsetHigh,
                     FILE_BEGIN);
      SetEndOfFile(h);
      CloseHandle(h);
#else
      int err = ::truncate(path.string().c_str(), size);
      if (err)
        throw boost::filesystem::filesystem_error(strerror(err), path,
          boost::system::error_code(err, boost::system::system_category()));
#endif
    }

    Buffer read_file_chunk(boost::filesystem::path path,
                           uint64_t offset,
                           uint64_t size)
    {
      // FIXME: accepts offsets >32bits, but size has to be below 1<<31
      // Fix for locale issue
      elle::os::locale::DefaultImbuer u;

      static const uint64_t MAX_buffer{elle::Buffer::max_size};

      if (size > MAX_buffer)
        throw elle::Exception(
          elle::sprintf("buffer that big (%s) can't be addressed", size));

      using namespace boost::system::errc; // make_error_code and errors
      /* Going Native! The iostream API does not give access to precise error
       * messages that the OS can provide. We need that information to
       * statistically measure the weird errors that appear in the wild.
       * Yes, it means C ressource management.
      */
#ifdef INFINIT_WINDOWS
      if (size > std::numeric_limits<DWORD>::max ())
        throw elle::Exception(
          elle::sprintf("buffer that big (%s) can't be read", size));
      HANDLE h = CreateFileW(
        std::wstring(path.string().begin(), path.string().end()).c_str(),
        GENERIC_READ,
        0, 0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);
      if (h == INVALID_HANDLE_VALUE)
      {
        throw boost::filesystem::filesystem_error(
          "CreateFile",
          path,
          boost::system::error_code(::GetLastError(),
                                    boost::system::system_category()));
      }
      elle::SafeFinally close_handle([&] { ::CloseHandle(h);});
      LONG offsetLow = offset & 0xFFFFFFFF;
      LONG offsetHigh = offset >> 32;
      DWORD seekAmount = SetFilePointer(h, offsetLow, &offsetHigh, FILE_BEGIN);
      DWORD err = GetLastError();
      if (seekAmount == INVALID_SET_FILE_POINTER && err != NO_ERROR)
      {
        throw boost::filesystem::filesystem_error(
          elle::sprintf("unable to seek to pos %s", offset),
          path,
          boost::system::error_code(::GetLastError()));
      }
      DWORD bytesRead;
      elle::Buffer buffer(size);
      BOOL ok = ReadFile(h, buffer.mutable_contents(), size, &bytesRead, NULL);
      if (ok != TRUE) // who knows what a WIN32 BOOL realy is?
      {
        throw boost::filesystem::filesystem_error(
          elle::sprintf("Read error: %s/%s", bytesRead, size)
          path,
          boost::system::error_code(::GetLastError()));
      }
      buffer.size(bytesRead);
      return buffer;
#else // POSIX version
#endif
      int fd = ::open(path.string().c_str(),/* trust the locale*/
                      O_RDONLY);
      if (fd == -1)
        throw boost::filesystem::filesystem_error(strerror(errno), path,
          boost::system::error_code(errno, boost::system::system_category()));
       elle::SafeFinally close_file([&] { ::close(fd);});
#ifdef INFINIT_MACOSX
      // macos has 64 bit lseek because off_t is 64 bits
      ELLE_ASSERT_GEQ(sizeof(off_t), 8);
      typedef off_t seek_offset_type;
      #define SEEK_FUNC ::lseek
#else
      // linux has lseek64
      typedef off64_t seek_offset_type;
      #define SEEK_FUNC ::lseek64
#endif
      seek_offset_type amount = SEEK_FUNC(fd, offset, SEEK_SET);
      if (amount == (seek_offset_type)-1)
        throw boost::filesystem::filesystem_error(strerror(errno), path,
          boost::system::error_code(errno, boost::system::system_category()));

      #undef SEEK_FUNC
      elle::Buffer buffer(size);
      // The spec does not explicitly say that a local FS read has to work in
      // one call :(
      size_t position = 0;
      while (position < size)
      {
        ssize_t bytes_read = ::read(fd,
                                    buffer.mutable_contents() + position,
                                    size - bytes_read);
        if (bytes_read < 0)
          throw boost::filesystem::filesystem_error(strerror(errno), path,
            boost::system::error_code(errno, boost::system::system_category()));
        else if (bytes_read == 0)
          break;
        position += static_cast<size_t>(bytes_read);
      }
      ELLE_ASSERT_GTE(size, position);
      buffer.size(position);
      return buffer;
    }
  }
}
