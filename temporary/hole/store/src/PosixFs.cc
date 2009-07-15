//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Fri Jan 23 15:17:04 2009 texane
// Last update Fri Jan 23 16:32:50 2009 texane
//



#ifdef _WIN32_WINNT
# include <windows.h>
# include <direct.h>
# include <io.h>
#else
# include <unistd.h>
# include <dirent.h>
#endif

#include <string>

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "Posix.hh"
#include "Debug.hh"

#include "pstdint.h"



using std::string;
using Posix::Fs::FileHandleT;
using Posix::ErrorT;



// exported
// notes: paths assumed to be translated


string Posix::Fs::concatPath(const string& lhs, const string& rhs)
{

#ifdef _WIN32_WINNT
# define PATH_SEPARATOR '\\'
#else
# define PATH_SEPARATOR '/'
#endif

  string res;

  size_t size = lhs.size();

  if (size)
    {
      res = lhs;

      if (lhs[size - 1] != PATH_SEPARATOR)
	res += PATH_SEPARATOR;
    }

  size = rhs.size();

  if (size)
    {
      if ((size != 1) && (rhs[0] == PATH_SEPARATOR))
	res.append(rhs, 1, size - 1);
      else if (rhs[0] != PATH_SEPARATOR)
	res.append(rhs, 0, size);
    }

  return res;
}


string Posix::Fs::translatePath(const string& path)
{
  // assume a unix like path

#ifdef _WIN32_WINNT

  // translate /c/foo/bar into c:\foo\bar

  string translatedPath = path;

  string::iterator pos = translatedPath.begin();
  string::iterator end = translatedPath.end();

  if ((end - pos) >= 3)
    {
      if (pos[0] == '/' && pos[2] == '/')
	{
	  pos[0] = pos[1];
	  pos[1] = ':';
	  pos[2] = '\\';
	}

      pos += 3;
    }
  
  while (pos != end)
    {
      if (*pos == '/')
	*pos = '\\';

      ++pos;
    }

  return translatedPath;

#else
  
  return path;

#endif
}


static ErrorT translateMode(int& oMode)
{
  ErrorT e = 0;

  switch (oMode)
    {
    case Posix::Fs::OMODE_RDONLY:
      oMode = O_RDONLY;
      break;

    case Posix::Fs::OMODE_WRONLY:
      oMode = O_WRONLY;
      break;

    case Posix::Fs::OMODE_RDWR:
      oMode = O_RDWR;
      break;

    default:
      e = EINVAL;
      break;
    }

  return e;
}


static ErrorT commonOpen(FileHandleT& handle, const string& path, int flags)
{
  // flags translated
  // use default perms

#ifdef _WIN32_WINNT
  handle = ::_open(path.c_str(), flags | O_BINARY, _S_IREAD | _S_IWRITE);
#else
  handle = ::open(path.c_str(), flags, S_IREAD | S_IWRITE);
#endif

  if (handle == -1)
    {
      const ErrorT error = errno;
      DEBUG_ERROR("open(%s) == %d\n", path.c_str(), error);
      return error;
    }
  
  return 0;
}


ErrorT
Posix::Fs::openFile(FileHandleT& handle, const string& path, int oMode)
{
  ErrorT error;

  handle = invalidFileHandle;

  error = translateMode(oMode);
  if (error)
    {
      DEBUG_ERROR("translateMode() == %d\n", error);
      return error;
    }

  return commonOpen(handle, path, oMode);
}


ErrorT
Posix::Fs::createFile(FileHandleT& handle, const string& path, int oMode)
{
  ErrorT error;

  handle = invalidFileHandle;

  error = translateMode(oMode);
  if (error)
    {
      DEBUG_ERROR("translateMode() == %d\n", error);
      return error;
    }

  return commonOpen(handle, path, oMode | O_CREAT);
}


void Posix::Fs::closeFile(FileHandleT& fileHandle)
{
#ifdef _WIN32_WINNT
  ::_close(fileHandle);
#else
  ::close(fileHandle);
#endif
}


ErrorT Posix::Fs::deleteFile(const string& path)
{
  ErrorT error;

#ifdef _WIN32_WINNT
  if (::_unlink(path.c_str()) != -1)
#else
  if (::unlink(path.c_str()) != -1)
#endif
    return 0;

  error = errno;

  DEBUG_ERROR("unlink(%s) == %d", path.c_str(), error);

  return error;
}


template<typename Tbuf, typename Tio>
static Posix::ErrorT doGenericIo(int fd, Tio io, Tbuf buf, size_t size)
{
#if _WIN32_WINNT
  int nio;
#else
  ssize_t nio;
#endif

#if _WIN32_WINNT
  if (::_lseek(fd, 0, SEEK_SET))
#else
  if (::lseek(fd, 0, SEEK_SET))
#endif
    return errno;

  while (size)
    {
      nio = io(fd, buf, size);
      if (nio <= 0)
	return errno;

      size -= nio;
      buf += nio;
    }

  return size ? EIO : 0;
}


ErrorT
Posix::Fs::writeFile(FileHandleT& h, const void* b, const FileSizeT& n)
{
#ifdef _WIN32_WINNT
  return doGenericIo(h, _write, static_cast<const uint8_t*>(b), n);
#else
  return doGenericIo(h, write, static_cast<const uint8_t*>(b), n);
#endif
}


ErrorT
Posix::Fs::readFile(FileHandleT& h, void* b, const FileSizeT& n)
{
#ifdef _WIN32_WINNT
  return doGenericIo(h, _read, static_cast<uint8_t*>(b), n);
#else
  return doGenericIo(h, read, static_cast<uint8_t*>(b), n);
#endif
}



bool Posix::Fs::isRegularFile(const std::string& path)
{
#ifdef _WIN32_WINNT
  struct ::_stat st;
  if (::_stat(path.c_str(), &st) == -1)
#else
  struct ::stat st;
  if (::stat(path.c_str(), &st) == -1)
#endif
    {
      DEBUG_ERROR("stat(%s) == %d\n", path.c_str(), errno);
      return false;
    }

#ifdef _WIN32_WINNT
  if (! ((st.st_mode & _S_IFMT) & _S_IFREG) )
#else
  if (! ((st.st_mode & S_IFMT) & S_IFREG) )
#endif
    return false;

  return true;
}


ErrorT
Posix::Fs::getFileSize(const FileHandleT& fileHandle, FileSizeT& fileSize)
{
#ifdef _WIN32_WINNT
  struct ::_stat st;
  if (::_fstat(fileHandle, &st) == -1)
#else
  struct stat st;
  if (::fstat(fileHandle, &st) == -1)
#endif
    {
      ErrorT error = errno;
      DEBUG_ERROR("fstat() == %d\n", errno);
      return error;
    }

  fileSize = st.st_size;

  return 0;
}


ErrorT
Posix::Fs::foreachEntry(const string& path, DirectoryVisitorT onFile, void* params)
{
  // flat (non recursive) walk
  // path the directory path
  // assume onFile not NULL

  ErrorT error = 0;

#ifdef _WIN32_WINNT

  // posix subsystem provides no routines
  // to manipulate directories on win32

  bool isDone = false;
  ::HANDLE findHandle = INVALID_HANDLE_VALUE;
  ::WIN32_FIND_DATA findData;
  string findPattern(path);

  findPattern += "\\*";

  findHandle = ::FindFirstFileA(findPattern.c_str(), &findData);
  if (findHandle == INVALID_HANDLE_VALUE)
    {
      DEBUG_ERROR("FindFirstFile(%s) == %u\n",
		  findPattern.c_str(),
		  GetLastError());

      error = EIO;
      goto onError;
    }

  while (!isDone)
    {
      if (onFile(findData.cFileName, params) == false)
	isDone = true;
      else if (::FindNextFile(findHandle, &findData) == FALSE)
	isDone = true;
    }

  ::FindClose(findHandle);

#else

  DIR* dir;
  struct dirent* de;
  bool isDone = false;

  if ((dir = ::opendir(path.c_str())) == NULL)
    {
      error = errno;
      DEBUG_ERROR("opendir(%s) == %d\n", path.c_str(), error);
      goto onError;
    }

  while (!isDone)
    {
      errno = 0;

      de = ::readdir(dir);
      if (de == NULL)
	{
	  error = errno;
	  isDone = true;
	}
      else if (onFile(de->d_name, params) == false)
	{
	  isDone = true;
	}
    }

  ::closedir(dir);

#endif  

 onError:

  return error;
}


ErrorT Posix::Fs::createDirectory(const string& path)
{
  // directory is created user wr able

#ifdef _WIN32_WINNT
  if (::_mkdir(path.c_str()) == -1)
#else
  if (::mkdir(path.c_str(), S_IRWXU) == -1)
#endif
    {
      ErrorT error = errno;
      DEBUG_ERROR("mkdir(%s) == %d\n", path.c_str(), error);
      return error;
    }

  return 0;
}


struct DeleteContext
{
  string dirPath_;
  ErrorT error_;

  DeleteContext(const string& dirPath)
    : dirPath_(dirPath), error_(0) {}
};


static bool deleteEntry(const string& name, void* params)
{
  struct DeleteContext* deleteContext =
    static_cast<struct DeleteContext*>(params);

  if (name == "." || name == "..")
    return true;

  string fullPath =
    Posix::Fs::concatPath(deleteContext->dirPath_, name);

  if (Posix::Fs::isRegularFile(fullPath) == false)
    {
      deleteContext->error_ = EPERM;
      return false;
    }

  deleteContext->error_ = Posix::Fs::deleteFile(fullPath);
  if (deleteContext->error_)
    return false;

  // continue

  return true;
} 


ErrorT Posix::Fs::deleteDirectory(const string& path)
{
  // assume flat directory

  struct DeleteContext deleteContext(path);

  ErrorT error = 0;

  // remove contents

  error = foreachEntry(path, deleteEntry, &deleteContext);
  if (error)
    return error;

  if (deleteContext.error_)
    return deleteContext.error_;

  // remove directory

#ifdef _WIN32_WINNT
  if (::_rmdir(path.c_str()) == -1)
#else
  if (::rmdir(path.c_str()) == -1)
#endif
    {
      error = errno;
      DEBUG_ERROR("rmdir(%s) == %d\n", path.c_str(), error);
      return error;
    }

  return 0;
}


bool Posix::Fs::isWritableDirectory(const string& path)
{
#ifdef _WIN32_WINNT
  struct ::_stat st;
  if (::_stat(path.c_str(), &st) == -1)
#else
  struct stat st;
  if (::stat(path.c_str(), &st) == -1)
#endif
    return false;

#ifdef _WIN32_WINNT
  if (! ((st.st_mode & _S_IFMT) & _S_IFDIR) )
#else
  if (! ((st.st_mode & S_IFMT) & S_IFDIR) )
#endif
    return false;

#ifdef _WIN32_WINNT
  if (! (st.st_mode & _S_IWRITE))
#else
  if (! (st.st_mode & S_IWRITE))
#endif
    return false;

  return true;
}
