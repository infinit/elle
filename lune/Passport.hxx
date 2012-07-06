#ifndef  LUNE_PASSPORT_HXX
# define LUNE_PASSPORT_HXX

# include <cassert>

# include <elle/cryptography/Signature.hh>
# include <hole/Label.hh>

ELLE_SERIALIZE_SIMPLE(lune::Passport,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.label;
  archive & value.id;
  archive & value.signature;
}

#endif
