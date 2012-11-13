#ifndef  LUNE_IDENTITY_HXX
# define LUNE_IDENTITY_HXX

# include <cassert>

# include <elle/serialize/Pointer.hh>
# include <cryptography/Cipher.hh>
# include <cryptography/Signature.hh>
# include <cryptography/KeyPair.hh>

ELLE_SERIALIZE_SIMPLE(lune::Identity,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & elle::serialize::pointer(value.cipher);
  if (value.cipher != nullptr)
    archive & value.pair;
  archive & value._id;
  archive & value.name;
  archive & value.signature;
}

#endif
