#include <elle/system/system.hh>

#include <boost/filesystem/fstream.hpp>

ELLE_LOG_COMPONENT("elle.system");

namespace elle
{
  namespace system
  {
    Buffer read_file_chunk(boost::filesystem::path path,
      int64_t offset,
      int64_t size)
    {
      boost::filesystem::ifstream file{path, std::ios::binary};
      // streamsize is garanteed signed, so this fits
      static const int64_t MAX_offset{
      std::numeric_limits<std::streamsize>::max()};
      static const size_t MAX_buffer{elle::Buffer::max_size};

      if (size > MAX_buffer)
        throw elle::Exception(
          elle::sprintf("buffer that big (%s) can't be addressed", size));

      if (!file.good())
        throw elle::Exception("file is broken");

      // If the offset is greater than the machine maximum streamsize, seekg n
      // times to reach the right offset.
      while (offset > MAX_offset)
      {
        file.seekg(MAX_offset, std::ios_base::cur);

        if (!file.good())
        throw elle::Exception(
          elle::sprintf("unable to increment offset by %s", MAX_offset));

        offset -= MAX_offset;
      }

      ELLE_DEBUG("seek to offset %s", offset);
      file.seekg(offset, std::ios_base::cur);

      if (!file.good())
      throw elle::Exception(
        elle::sprintf("unable to seek to pos %s", offset));

      elle::Buffer buffer(size);

      ELLE_DEBUG("read file");
      file.read(reinterpret_cast<char*>(buffer.mutable_contents()), size);
      buffer.size(file.gcount());

      ELLE_DEBUG("buffer resized to %s bytes", buffer.size());

      if (!file.eof() && file.fail() || file.bad())
        throw elle::Exception("unable to reathd");

      ELLE_DUMP("buffer read: %x", buffer);
      return buffer;
    }
  }
}
