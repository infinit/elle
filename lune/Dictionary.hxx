#ifndef  LUNE_DICTIONARY_HXX
# define LUNE_DICTIONARY_HXX

# include <cassert>

# include <cryptography/PublicKey.hh>
# include <nucleus/proton/Address.hh>
# include <lune/Map.hh>

ELLE_SERIALIZE_SIMPLE(lune::Dictionary,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.users;
  archive & value.groups;
}

#endif
