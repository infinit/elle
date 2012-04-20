#ifndef  ELLE_SERIALIZE_REGION_SERIALIZER_HXX
# define ELLE_SERIALIZE_REGION_SERIALIZER_HXX

# include <elle/utility/BufferSerializer.hxx>

# include <elle/standalone/Region.hh>

ELLE_SERIALIZE_SPLIT(elle::standalone::Region)

ELLE_SERIALIZE_SPLIT_SAVE(elle::standalone::Region,
                          archive,
                          value,
                          version)
{
  archive << elle::utility::WeakBuffer(value.contents, value.size);
}

ELLE_SERIALIZE_SPLIT_LOAD(elle::standalone::Region,
                          archive,
                          value,
                          version)
{
  elle::utility::Buffer buffer;
  archive >> buffer;
  auto data = buffer.Release();
  value.Acquire(data.first.get(), data.second);
  data.first.release();
}

#endif
