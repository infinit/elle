#ifndef  ETOILE_GEAR_CONTEXTSERIALIZER_HXX
# define ETOILE_GEAR_CONTEXTSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <etoile/gear/Context.hh>

ELLE_SERIALIZE_SIMPLE(etoile::gear::Context,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.nature;
  archive & value.state;
}

#endif
