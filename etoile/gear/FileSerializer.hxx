#ifndef  ETOILE_GEAR_FILESERIALIZER_HXX
# define ETOILE_GEAR_FILESERIALIZER_HXX

# include <cassert>

# include <elle/serialize/PointerSerializer.hxx>

# include <nucleus/neutron/DataSerializer.hxx>
# include <nucleus/proton/ContentsSerializer.hxx>

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
