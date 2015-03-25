#ifndef INFINIT_CRYPTOGRAPHY_RSA_KEYPAIR_HXX
# define INFINIT_CRYPTOGRAPHY_RSA_KEYPAIR_HXX

//
// ---------- Class -----------------------------------------------------------
//

/*-------------.
| Serializable |
`-------------*/

# include <cryptography/Exception.hh>
# include <cryptography/rsa/PublicKey.hh>
# include <cryptography/rsa/PrivateKey.hh>

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SPLIT(infinit::cryptography::rsa::KeyPair);

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::rsa::KeyPair,
                          archive,
                          value,
                          format)
{
  ELLE_ASSERT_NEQ(value._K, nullptr);
  ELLE_ASSERT_NEQ(value._k, nullptr);

  archive << *value._K;
  archive << *value._k;
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::rsa::KeyPair,
                          archive,
                          value,
                          format)
{
  value._K.reset(new infinit::cryptography::rsa::PublicKey);
  value._k.reset(new infinit::cryptography::rsa::PrivateKey);

  archive >> *value._K;
  archive >> *value._k;

  ELLE_ASSERT_NEQ(value._K, nullptr);
  ELLE_ASSERT_NEQ(value._k, nullptr);
}

//
// ---------- Hash ------------------------------------------------------------
//

namespace std
{
  template <>
  struct hash<infinit::cryptography::rsa::KeyPair>
  {
    size_t
    operator ()(infinit::cryptography::rsa::KeyPair const& value) const
    {
      return (std::hash<infinit::cryptography::rsa::PublicKey>()(value.K()));
    }
  };
}

#endif
