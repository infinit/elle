#ifndef  ELLE_NETWORK_HEADERSERIALIZER_HXX
# define ELLE_NETWORK_HEADERSERIALIZER_HXX

# include <elle/print.hh>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/concurrency/Event.hh>
# include <elle/network/Header.hh>

ELLE_SERIALIZE_SPLIT(elle::network::Header);

ELLE_SERIALIZE_SPLIT_SAVE(elle::network::Header,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
  archive & value.event;
  archive & value.tag;
  archive & value.size;
}

ELLE_SERIALIZE_SPLIT_LOAD(elle::network::Header,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
  int i;
  elle::print(++i, "Offset: ", archive.Stream().Offset(), archive.Stream().BytesLeft());
  archive & value.event;
  elle::print(++i, "Offset: ", archive.Stream().Offset(), archive.Stream().BytesLeft());
  archive & value.tag;
  elle::print(++i, "Offset: ", archive.Stream().Offset(), archive.Stream().BytesLeft());
  archive & value.size;
  elle::print(++i, "Offset: ", archive.Stream().Offset(), archive.Stream().BytesLeft());
}

#endif
