#ifndef  ELLE_TEST_NETWORK_UDP_NEIGHBOURSERIALIZER_HXX
# define ELLE_TEST_NETWORK_UDP_NEIGHBOURSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/network/Locus.hh>

# include <elle/test/network/udp/Neighbour.hh>

ELLE_SERIALIZE_SIMPLE(elle::test::Neighbour,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
  archive & value.locus;
  archive & value.name;
}

#endif
