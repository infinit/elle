#ifndef  HOLE_MODELSERIALIZER_HXX
# define HOLE_MODELSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <hole/Model.hh>

ELLE_SERIALIZE_SIMPLE(hole::Model,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.type;
}

#endif
