#ifndef  ELLE_NETWORK_HEADER_HXX
# define ELLE_NETWORK_HEADER_HXX

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
  archive & value.event;
  archive & value.tag;
  archive & value.size;
}

#endif
