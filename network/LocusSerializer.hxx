#ifndef  ELLE_NETWORK_LOCUSSERIALIZER_HXX
# define ELLE_NETWORK_LOCUSSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/network/HostSerializer.hxx>

# include <elle/network/Locus.hh>

ELLE_SERIALIZE_SIMPLE(elle::network::Locus,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.host;
  archive & value.port;
}

#endif
