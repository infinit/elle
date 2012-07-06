#ifndef  ETOILE_GEAR_DIRECTORY_HXX
# define ETOILE_GEAR_DIRECTORY_HXX

# include <cassert>

# include <elle/serialize/Pointer.hh>

# include <nucleus/proton/Contents.hh>
# include <nucleus/neutron/Catalog.hh>

ELLE_SERIALIZE_SIMPLE(etoile::gear::Directory,
                      archive,
                      value,
                      version)
{
  archive & elle::serialize::pointer(value.contents);
}

#endif
