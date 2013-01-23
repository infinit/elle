#ifndef ETOILE_ABSTRACT_GROUP_HXX
# define ETOILE_ABSTRACT_GROUP_HXX

# include <elle/serialize/Pointer.hh>
# include <elle/utility/Time.hh>

# include <cryptography/PublicKey.hh>

ELLE_SERIALIZE_SIMPLE(etoile::abstract::Group,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.description;
  archive & value.timestamps.creation;
  archive & value.timestamps.modification;
  archive & value.size;
  archive & elle::serialize::alive_pointer(value.manager);
  archive & value.revision;
  archive & elle::serialize::alive_pointer(value.pass_K);
}

#endif
