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
  enforce(version == 0);

  archive & value.genre;
  archive & value.timestamps.creation;
  archive & value.timestamps.modification;
  archive & value.size;
  archive & value.keys.owner;
  archive & value.keys.author;
  archive & value.permissions.owner;
  archive & value.revisions.meta;
  archive & value.revisions.data;
}

#endif
