#ifndef  ETOILE_GEAR_LINKSERIALIZER_HXX
# define ETOILE_GEAR_LINKSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/PointerSerializer.hxx>

# include <nucleus/neutron/ReferenceSerializer.hxx>
# include <nucleus/proton/ContentsSerializer.hxx>

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
