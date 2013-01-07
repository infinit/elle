#ifndef LUNE_IDENTITY_HXX
# define LUNE_IDENTITY_HXX

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

  // XXX[way of serializing the identity in its decrypted form:
  //     not very elegant: to improve!]
  if (value.cipher != nullptr)
    archive & elle::serialize::alive_pointer(value._pair);

  archive & value._id;
  archive & value.name;
  archive & elle::serialize::alive_pointer(value._signature);
}

#endif
