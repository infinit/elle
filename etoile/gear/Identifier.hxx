#ifndef  ETOILE_GEAR_IDENTIFIER_HXX
# define ETOILE_GEAR_IDENTIFIER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SIMPLE(etoile::gear::Identifier,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.value;
}

#endif
