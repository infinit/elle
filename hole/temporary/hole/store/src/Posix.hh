//
// Made by flementec@rd.skyrecon.com
// 
// Started on  Thu Jan 22 08:27:30 2009 flementec
// Last update Fri Jan 23 08:41:01 2009 flementec
//



#ifndef POSIX_HH_INCLUDED
# define POSIX_HH_INCLUDED



#ifdef _WIN32_WINNT

#include <windows.h>
#include <io.h>

#endif

#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>

#include <string>



namespace Posix
{
  typedef int ErrorT;

  static inline
  bool isSuccess(const ErrorT& e) { return e ? false : true ; }

  namespace Fs
  {
    // types

    typedef int FileHandleT;
    typedef size_t FileSizeT;
    typedef off_t FileOffsetT;

    static const FileHandleT invalidFileHandle = -1;

    // open modes

    enum
      {
	OMODE_RDONLY = 0,
	OMODE_WRONLY,
	OMODE_RDWR
      };

    // path translation

    std::string translatePath(const std::string&);
    std::string concatPath(const std::string&, const std::string&);

    // file ios

    ErrorT openFile(FileHandleT&, const std::string&, int = OMODE_RDWR);
    ErrorT createFile(FileHandleT&, const std::string&, int = OMODE_RDWR);
    void closeFile(FileHandleT&);
    ErrorT deleteFile(const std::string&);

    ErrorT writeFile(FileHandleT&, const void*, const FileSizeT&);
    ErrorT readFile(FileHandleT&, void*, const FileSizeT&);

    bool isRegularFile(const std::string&);
    ErrorT getFileSize(const FileHandleT&, FileSizeT&);

    // walk directory

    typedef bool (*DirectoryVisitorT)(const std::string&, void*);

    ErrorT foreachEntry(const std::string&, DirectoryVisitorT, void* = NULL);

    // directory ops

    ErrorT createDirectory(const std::string&);
    ErrorT deleteDirectory(const std::string&);
    bool isWritableDirectory(const std::string&);
  }
}



#endif // ! POSIX_HH_INCLUDED
