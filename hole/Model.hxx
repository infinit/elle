#ifndef  HOLE_MODEL_HXX
# define HOLE_MODEL_HXX

# include <cassert>

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(hole::Model,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.type;
}

#endif
