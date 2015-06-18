#ifndef INFINIT_CRYPTOGRAPHY_DH_KEYPAIR_HXX
# define INFINIT_CRYPTOGRAPHY_DH_KEYPAIR_HXX

//
// ---------- Class -----------------------------------------------------------
//

/*-------------.
| Serializable |
`-------------*/

# include <cryptography/Exception.hh>
# include <cryptography/dh/PublicKey.hh>
# include <cryptography/dh/PrivateKey.hh>

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SPLIT(infinit::cryptography::dh::KeyPair);

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::dh::KeyPair,
                          archive,
                          value,
                          format)
{
  ELLE_ASSERT_NEQ(value._K, nullptr);
  ELLE_ASSERT_NEQ(value._k, nullptr);

  archive << *value._K;
  archive << *value._k;
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::dh::KeyPair,
                          archive,
                          value,
                          format)
{
  value._K.reset(new infinit::cryptography::dh::PublicKey);
  value._k.reset(new infinit::cryptography::dh::PrivateKey);

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
  struct hash<infinit::cryptography::dh::KeyPair>
  {
    size_t
    operator ()(infinit::cryptography::dh::KeyPair const& value) const
    {
      return (std::hash<infinit::cryptography::dh::PublicKey>()(value.K()));
    }
  };
}

#endif
