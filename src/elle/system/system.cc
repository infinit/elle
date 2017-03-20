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

#include <boost/system/error_code.hpp>

namespace elle
{
  namespace system
  {
    void
    truncate(boost::filesystem::path path,
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
        auto error = ::GetLastError();
        throw boost::filesystem::filesystem_error(
          "CreateFile",
          path,
          boost::system::error_code(error, boost::system::system_category()));
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

    void
    write_file(boost::filesystem::path const& path,
               Buffer const& buffer)
    {
      FileHandle(path, FileHandle::APPEND).write(buffer);
    }

    void
    FileHandle::write(Buffer const& buffer)
    {
#ifdef INFINIT_WINDOWS

      DWORD written;
      BOOL ok =
        WriteFile(_handle, buffer.contents(), buffer.size(), &written, nullptr);
      if (ok != TRUE)
      {
        auto error = ::GetLastError();
        throw boost::filesystem::filesystem_error(
          elle::sprintf("Write error: %s/%s", written, buffer.size()),
          path(),
          boost::system::error_code(error, boost::system::system_category()));
      }
#else
      ssize_t bytes_written = ::write(_handle,
                                      buffer.contents(),
                                      buffer.size());
      if (bytes_written == -1 || (Buffer::Size) bytes_written != buffer.size())
        throw boost::filesystem::filesystem_error(
          strerror(errno), _path,
          boost::system::error_code(errno, boost::system::system_category()));
#endif
      if (!boost::filesystem::exists(_path))
        throw boost::filesystem::filesystem_error("File vanished", _path,
          boost::system::error_code(ENOENT, boost::system::system_category()));
    }

    Buffer
    read_file_chunk(boost::filesystem::path path,
                    uint64_t offset,
                    uint64_t size)
    {
      return FileHandle(path, FileHandle::READ).read(offset, size);
    }

    FileHandle::FileHandle(boost::filesystem::path const& path, OpenMode mode)
      : _path(path)
    {
#ifdef INFINIT_WINDOWS
      ELLE_LOG_COMPONENT("elle.system");
      ELLE_DEBUG("get HANDLE for file: %s", path);
      switch(mode)
      {
      case READ:
       _handle = CreateFileW(
        path.native().c_str(),
        GENERIC_READ,
        FILE_SHARE_READ, 0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);
       break;
     case WRITE:
       _handle =  CreateFileW(
          path.native().c_str(),
          GENERIC_WRITE,
          FILE_SHARE_READ,
          NULL,
          OPEN_ALWAYS,
          FILE_ATTRIBUTE_NORMAL,
          NULL);
       break;
     case APPEND:
       _handle = CreateFileW(
          path.native().c_str(),
          FILE_APPEND_DATA,
          FILE_SHARE_READ,
          NULL,
          OPEN_ALWAYS,
          FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
          NULL);
       break;
      }
      if (_handle == INVALID_HANDLE_VALUE)
      {
        auto error = ::GetLastError();
        throw boost::filesystem::filesystem_error(
          "CreateFile",
          path,
          boost::system::error_code(error, boost::system::system_category()));
      }
#else
      int flags = 0;
      switch (mode)
      {
      case READ:
        flags = O_RDONLY;
        break;
      case WRITE:
        flags = O_WRONLY | O_CREAT;
        break;
      case APPEND:
        flags = O_WRONLY | O_CREAT | O_APPEND;
        break;
      }
      mode_t m = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
      _handle = ::open(path.string().c_str(),/* trust the locale*/
                      flags, m);
      if (_handle == -1)
        throw boost::filesystem::filesystem_error(strerror(errno), path,
          boost::system::error_code(errno, boost::system::system_category()));
#endif
    }
    FileHandle::NativeHandle FileHandle::_invalid =
#ifdef INFINIT_WINDOWS
    INVALID_HANDLE_VALUE;
#else
    -1;
#endif
    FileHandle::FileHandle()
      : _handle(_invalid)
    {
    }
    FileHandle::FileHandle(FileHandle && b)
      : _handle(_invalid)
    {
      *this = std::move(b);
    }
    FileHandle& FileHandle::operator= (FileHandle&& b)
    {
      if (_handle != _invalid)
#ifdef INFINIT_WINDOWS
        ::CloseHandle(_handle);
#else
        ::close(_handle);
#endif
      _path = b._path;
      b._path = boost::filesystem::path();
      _handle = b._handle;
      b._handle = _invalid;
      return *this;
    }
    FileHandle::~FileHandle()
    {
      if (_handle != _invalid)
#ifdef INFINIT_WINDOWS
        ::CloseHandle(_handle);
#else
        ::close(_handle);
#endif
    }
    Buffer
    FileHandle::read(uint64_t offset, uint64_t size)
    {
#ifdef INFINIT_WINDOWS
      LONG offsetLow = offset & 0xFFFFFFFF;
      LONG offsetHigh = offset >> 32;
      DWORD seekAmount = SetFilePointer(_handle, offsetLow, &offsetHigh, FILE_BEGIN);
      auto error = ::GetLastError();
      if (seekAmount == INVALID_SET_FILE_POINTER && error != NO_ERROR)
      {
        throw boost::filesystem::filesystem_error(
          elle::sprintf("unable to seek to pos %s", offset),
          path(),
          boost::system::error_code(error, boost::system::system_category()));
      }
#else // POSIX version
#if defined(INFINIT_MACOSX) || defined(INFINIT_IOS)
      // macos has 64 bit lseek because off_t is 64 bits
      ELLE_ASSERT_GTE(sizeof(off_t), 8u);
      using seek_offset_type = off_t;
      #define SEEK_FUNC ::lseek
#else
      // linux has lseek64
      using seek_offset_type = off64_t;
      #define SEEK_FUNC ::lseek64
#endif
      seek_offset_type amount = SEEK_FUNC(_handle, offset, SEEK_SET);
      if (amount == (seek_offset_type)-1)
        throw boost::filesystem::filesystem_error(strerror(errno), _path,
          boost::system::error_code(errno, boost::system::system_category()));
#undef SEEK_FUNC
#endif
      return read(size);
    }
    Buffer
    FileHandle::read(uint64_t size)
    {
#ifdef INFINIT_WINDOWS
      DWORD bytesRead;
      elle::Buffer buffer(size);
      BOOL ok = ReadFile(_handle, buffer.mutable_contents(), size, &bytesRead, NULL);
      if (ok != TRUE) // who knows what a WIN32 BOOL realy is?
      {
        auto error = ::GetLastError();
        throw boost::filesystem::filesystem_error(
          elle::sprintf("Read error: %s/%s", bytesRead, size),
          path(),
          boost::system::error_code(error, boost::system::system_category()));
      }
      buffer.size(bytesRead);
      return buffer;
#else
      elle::Buffer buffer(size);
      // The spec does not explicitly say that a local FS read has to work in
      // one call :(
      size_t position = 0;
      while (position < size)
      {
        ssize_t bytes_read = ::read(_handle,
                                    buffer.mutable_contents() + position,
                                    size - position);
        if (bytes_read < 0)
          throw boost::filesystem::filesystem_error(strerror(errno), _path,
            boost::system::error_code(errno, boost::system::system_category()));
        else if (bytes_read == 0)
          break;
        position += static_cast<size_t>(bytes_read);
      }
      ELLE_ASSERT_GTE(size, position);
      buffer.size(position);
      return buffer;
#endif // INFINIT_WINDOWS
    }

  }
}
