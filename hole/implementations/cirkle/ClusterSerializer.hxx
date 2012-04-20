#ifndef  HOLE_IMPLEMENTATIONS_CIRKLE_CLUSTERSERIALIZER_HXX
# define HOLE_IMPLEMENTATIONS_CIRKLE_CLUSTERSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ListSerializer.hxx>
# include <elle/network/LocusSerializer.hxx>

# include <hole/implementations/cirkle/Cluster.hh>

ELLE_SERIALIZE_SIMPLE(hole::implementations::cirkle::Cluster,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.container;
}

#endif
