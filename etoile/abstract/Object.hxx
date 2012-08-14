#ifndef ETOILE_ABSTRACT_OBJECT_HXX
# define ETOILE_ABSTRACT_OBJECT_HXX

# include <cassert>

# include <elle/cryptography/PublicKey.hh>
# include <elle/utility/Time.hh>

ELLE_SERIALIZE_SIMPLE(etoile::abstract::Object,
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
