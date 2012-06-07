#ifndef  HOLE_LABELSERIALIZER_HXX
# define HOLE_LABELSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/Pointer.hh>
# include <elle/cryptography/DigestSerializer.hxx>

# include <hole/Label.hh>

ELLE_SERIALIZE_SIMPLE(hole::Label,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & elle::serialize::pointer(value.digest);
}

#endif
