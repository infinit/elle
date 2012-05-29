#ifndef  ETOILE_GEAR_DIRECTORYSERIALIZER_HXX
# define ETOILE_GEAR_DIRECTORYSERIALIZER_HXX

# include <cassert>

# include <elle/serializer/PointerSerializer.hxx>

# include <nucleus/proton/ContentsSerializer.hxx>
# include <nucleus/neutron/CatalogSerializer.hxx>

# include <etoile/gear/Directory.hh>

ELLE_SERIALIZE_SIMPLE(etoile::gear::Directory,
                      archive,
                      value,
                      version)
{
  archive & elle::serialize::pointer(value.contents);
}

#endif
