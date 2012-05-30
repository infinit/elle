#ifndef  ELLE_SERIALIZE_REGION_SERIALIZER_HXX
# define ELLE_SERIALIZE_REGION_SERIALIZER_HXX

# include <iostream>

# include <elle/utility/BufferSerializer.hxx>

# include <elle/standalone/Region.hh>

ELLE_SERIALIZE_SPLIT(elle::standalone::Region)

ELLE_SERIALIZE_SPLIT_SAVE(elle::standalone::Region,
                          archive,
                          value,
                          version)
{
  archive << static_cast<uint64_t>(value.size);
  archive.SaveBinary(value.contents, value.size);
  std::cout << "Saved region of " << value.size << " bytes\n";
}

ELLE_SERIALIZE_SPLIT_LOAD(elle::standalone::Region,
                          archive,
                          value,
                          version)
{
  uint64_t size;
  archive >> size;

  elle::utility::Buffer buffer(size);
  archive.LoadBinary(buffer.MutableContents(), size);

  auto ptr = buffer.Release().first;
  if (value.Acquire(ptr.get(), size) == elle::Status::Error)
    throw std::runtime_error("Cannot acquire the pointer!");
  ptr.release();

  std::cout << "Loaded region of " << size << " bytes\n";
}

#endif
