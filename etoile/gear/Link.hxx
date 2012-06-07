#ifndef  ETOILE_GEAR_LINKSERIALIZER_HXX
# define ETOILE_GEAR_LINKSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/Pointer.hh>

# include <nucleus/neutron/Reference.hh>
# include <nucleus/proton/Contents.hh>

# include <etoile/gear/Link.hh>

ELLE_SERIALIZE_SIMPLE(etoile::gear::Link,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & elle::serialize::pointer(value.contents);
}

#endif
