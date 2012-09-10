#ifndef  ETOILE_GEAR_LINK_HXX
# define ETOILE_GEAR_LINK_HXX

# include <cassert>

# include <elle/serialize/Pointer.hh>

# include <nucleus/neutron/Reference.hh>
# include <nucleus/proton/Contents.hh>

ELLE_SERIALIZE_SIMPLE(etoile::gear::Link,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & elle::serialize::pointer(value.contents);
}

#endif
