#ifndef  ETOILE_MISCELLANEOUS_ABSTRACTSERIALIZER_HXX
# define ETOILE_MISCELLANEOUS_ABSTRACTSERIALIZER_HXX

# include <cassert>

# include <elle/cryptography/PublicKeySerializer.hxx>
# include <elle/utility/TimeSerializer.hxx>

# include <etoile/miscellaneous/Abstract.hh>

ELLE_SERIALIZE_SIMPLE(etoile::miscellaneous::Abstract,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.genre;
  archive & value.stamps.creation;
  archive & value.stamps.modification;
  archive & value.size;
  archive & value.keys.owner;
  archive & value.keys.author;
  archive & value.permissions.owner;
  archive & value.versions.meta;
  archive & value.versions.data;
}

#endif
