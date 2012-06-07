#ifndef  ETOILE_GEAR_IDENTIFIERSERIALIZER_HXX
# define ETOILE_GEAR_IDENTIFIERSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <etoile/gear/Identifier.hh>

ELLE_SERIALIZE_SIMPLE(etoile::gear::Identifier,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.value;
}

#endif
