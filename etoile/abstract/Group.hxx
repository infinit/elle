#ifndef ETOILE_ABSTRACT_GROUP_HXX
# define ETOILE_ABSTRACT_GROUP_HXX

# include <cassert>

# include <elle/cryptography/PublicKey.hh>
# include <elle/utility/Time.hh>

ELLE_SERIALIZE_SIMPLE(etoile::abstract::Group,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.description;
  archive & value.stamps.creation;
  archive & value.stamps.modification;
  archive & value.size;
  archive & value.manager;
  archive & value.version;
  archive & value.pass_K;
}

#endif
