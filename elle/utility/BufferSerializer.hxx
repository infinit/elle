#ifndef  ELLE_UTILITY_BUFFERSERIALIZER_HXX
# define ELLE_UTILITY_BUFFERSERIALIZER_HXX

# include <stdexcept>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <elle/utility/Buffer.hh>

ELLE_SERIALIZE_SPLIT(elle::utility::Buffer)

ELLE_SERIALIZE_SPLIT_LOAD(elle::utility::Buffer,
                          archive,
                          value,
                          version)
{
  uint64_t size;
  archive >> size;
  if (sizeof(uint64_t) > sizeof(size_t))
    {
      if (size >= static_cast<size_t>(-1))
        throw std::runtime_error("Cannot receive a buffer that large");
    }
  value.Size(size);
  archive.LoadBinary(value._contents, size);
}

#endif
