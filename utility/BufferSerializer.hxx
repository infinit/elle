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
  assert(version == 0);
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

ELLE_SERIALIZE_SPLIT(elle::utility::WeakBuffer)

ELLE_SERIALIZE_SPLIT_SAVE(elle::utility::WeakBuffer,
                          archive,
                          value,
                          version)
{
  assert(version == 0);
  archive << value.Size();
  archive.SaveBinary(value.Contents(), value.Size());
}

#endif
