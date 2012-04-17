#ifndef  ELLE_NETWORK_HEADERSERIALIZER_HXX
# define ELLE_NETWORK_HEADERSERIALIZER_HXX

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/concurrency/EventSerializer.hxx>
# include <elle/network/Header.hh>

ELLE_SERIALIZE_SIMPLE(elle::network::Header,
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
