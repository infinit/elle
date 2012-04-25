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
  std::cout << "Saving region of " << value.size << std::endl;
  archive << static_cast<uint64_t>(value.size);
  archive.SaveBinary(value.contents, value.size);
}

ELLE_SERIALIZE_SPLIT_LOAD(elle::standalone::Region,
                          archive,
                          value,
                          version)
{
  uint64_t size;
  archive >> size;
  std::cout << "Loading region of " << size << std::endl;
  if (value.Prepare(size) == elle::Status::Error)
    throw std::bad_cast();
  archive.LoadBinary(value.contents, value.size);
  value.size = size;
}

#endif
