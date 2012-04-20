#ifndef  HOLE_IMPLEMENTATIONS_SLUG_CLUSTERSERIALIZER_HXX
# define HOLE_IMPLEMENTATIONS_SLUG_CLUSTERSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ListSerializer.hxx>
# include <elle/network/LocusSerializer.hxx>

# include <hole/implementations/slug/Cluster.hh>

ELLE_SERIALIZE_SIMPLE(hole::implementations::slug::Cluster,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.container;
}

#endif
