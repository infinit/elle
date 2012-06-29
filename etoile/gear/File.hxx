#ifndef  ETOILE_GEAR_FILE_HXX
# define ETOILE_GEAR_FILE_HXX

# include <cassert>

# include <elle/serialize/Pointer.hh>

# include <nucleus/neutron/Data.hh>
# include <nucleus/proton/Contents.hh>

# include <etoile/gear/File.hh>

ELLE_SERIALIZE_SIMPLE(etoile::gear::File,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & elle::serialize::pointer(value.contents);
}

#endif
