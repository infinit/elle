#ifndef  LUNE_PASSPORT_HXX
# define LUNE_PASSPORT_HXX

# include <cassert>

# include <elle/cryptography/Signature.hh>
# include <hole/Label.hh>

ELLE_SERIALIZE_SIMPLE(elle::Passport,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.label;
  archive & value.id;
  archive & value.signature;
}

#endif
