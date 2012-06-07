#ifndef  LUNE_DICTIONARYSERIALIZER_HXX
# define LUNE_DICTIONARYSERIALIZER_HXX

# include <cassert>

# include <elle/cryptography/PublicKey.hh>
# include <nucleus/proton/Address.hh>
# include <lune/Map.hh>

# include <lune/Dictionary.hh>

ELLE_SERIALIZE_SIMPLE(lune::Dictionary,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.users;
  archive & value.groups;
}

#endif
