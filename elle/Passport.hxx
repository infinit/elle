#ifndef  LUNE_PASSPORT_HXX
# define LUNE_PASSPORT_HXX

ELLE_SERIALIZE_SIMPLE(elle::Passport,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._id;
  archive & value._name;
  archive & value._owner_K;
  archive & value._signature;
}

#endif
