#ifndef INFINIT_CRYPTOGRAPHY_DSA_KEYPAIR_HXX
# define INFINIT_CRYPTOGRAPHY_DSA_KEYPAIR_HXX

//
// ---------- Class -----------------------------------------------------------
//

/*-------------.
| Serializable |
`-------------*/

# include <cryptography/Exception.hh>
# include <cryptography/dsa/PublicKey.hh>
# include <cryptography/dsa/PrivateKey.hh>

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SPLIT(infinit::cryptography::dsa::KeyPair);

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::dsa::KeyPair,
                          archive,
                          value,
                          format)
{
  ELLE_ASSERT_NEQ(value._K, nullptr);
  ELLE_ASSERT_NEQ(value._k, nullptr);

  archive << *value._K;
  archive << *value._k;
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::dsa::KeyPair,
                          archive,
                          value,
                          format)
{
  value._K.reset(new infinit::cryptography::dsa::PublicKey);
  value._k.reset(new infinit::cryptography::dsa::PrivateKey);

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
  struct hash<infinit::cryptography::dsa::KeyPair>
  {
    size_t
    operator ()(infinit::cryptography::dsa::KeyPair const& value) const
    {
      return (std::hash<infinit::cryptography::dsa::PublicKey>()(value.K()));
    }
  };
}

#endif
