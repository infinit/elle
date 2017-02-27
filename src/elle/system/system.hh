#pragma once

#include <boost/filesystem.hpp>

#include <elle/Buffer.hh>
#include <elle/system/platform.hh>

namespace elle
{
  /// This namespace contains system-related stuff especially useful
  /// when it comes to portability.
  namespace system
  {
    /*-------------.
    | Enumerations |
    `-------------*/
    enum class Endianness
    {
      little = 0x1,
      big,
      middle,
    };

    /*----------.
    | Constants |
    `----------*/
#if defined(BOOST_BIG_ENDIAN)
    static Endianness const endianness(Endianness::big);
#elif defined(BOOST_LITTLE_ENDIAN)
    static Endianness const endianness(Endianness::little);
#elif defined(BOOST_PDP_ENDIAN)
    static Endianness const endianness(Endianness::middle);
#endif

    namespace path
    {
      static char const separator('/');
    };
    /// Read a part of a file, from file_offset to either size bytes or EOF
    Buffer
    read_file_chunk(boost::filesystem::path file_name,
                    uint64_t file_offset,
                    uint64_t size);

    /// Write at the ond of the file
    void
    write_file(boost::filesystem::path const& path,
               Buffer const& buffer = elle::Buffer(0));

    void
    truncate(boost::filesystem::path file_name,
             uint64_t new_size);

    class FileHandle
    {
    public:
      enum OpenMode
      {
        READ,
        WRITE,
        APPEND
      };
      FileHandle();
      FileHandle(boost::filesystem::path, OpenMode mode);
      FileHandle(FileHandle && b);
      FileHandle(FileHandle const&) = delete;
      FileHandle& operator=(FileHandle const&) = delete;
      FileHandle& operator=(FileHandle&& b);
      ~FileHandle();
      void
      write(Buffer const& buffer);
      Buffer
      read(uint64_t size);
      Buffer
      read(uint64_t file_offset, uint64_t size);
    private:
#ifdef INFINIT_WINDOWS
    using NativeHandle = HANDLE;
#else
     using NativeHandle = int;
#endif
      NativeHandle _handle;
      static NativeHandle _invalid;
      ELLE_ATTRIBUTE_R(boost::filesystem::path, path);
    };

  }
}
