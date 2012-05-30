#ifndef  LUNE_PASSPORTSERIALIZER_HXX
# define LUNE_PASSPORTSERIALIZER_HXX

# include <cassert>

# include <elle/cryptography/SignatureSerializer.hxx>
# include <hole/LabelSerializer.hxx>

# include <lune/Passport.hh>

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
