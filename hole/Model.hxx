#ifndef  HOLE_MODEL_HXX
# define HOLE_MODEL_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SIMPLE(hole::Model,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.type;
}

#endif
