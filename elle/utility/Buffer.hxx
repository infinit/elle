#ifndef  ELLE_UTILITY_BUFFER_HXX
# define ELLE_UTILITY_BUFFER_HXX

# include <stdexcept>

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SPLIT(elle::utility::Buffer)

ELLE_SERIALIZE_SPLIT_LOAD(elle::utility::Buffer,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);
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

ELLE_SERIALIZE_SPLIT_SAVE(elle::utility::Buffer,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);
  archive << static_cast<uint64_t>(value.Size());
  archive.SaveBinary(value.Contents(), value.Size());
}


ELLE_SERIALIZE_SPLIT(elle::utility::WeakBuffer)

ELLE_SERIALIZE_SPLIT_SAVE(elle::utility::WeakBuffer,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);
  archive << static_cast<uint64_t>(value.Size());
  archive.SaveBinary(value.Contents(), value.Size());
}

ELLE_SERIALIZE_SPLIT_LOAD(elle::utility::WeakBuffer,,,)
{
  throw false;
}

#endif
