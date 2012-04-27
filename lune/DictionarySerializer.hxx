#ifndef  LUNE_DICTIONARYSERIALIZER_HXX
# define LUNE_DICTIONARYSERIALIZER_HXX

# include <cassert>

# include <elle/cryptography/PublicKeySerializer.hxx>
# include <nucleus/proton/AddressSerializer.hxx>
# include <lune/MapSerializer.hxx>

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
