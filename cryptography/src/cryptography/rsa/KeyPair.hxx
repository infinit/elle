#ifndef INFINIT_CRYPTOGRAPHY_RSA_KEYPAIR_HXX
# define INFINIT_CRYPTOGRAPHY_RSA_KEYPAIR_HXX

//
// ---------- Class -----------------------------------------------------------
//

# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)

/*-------------.
| Serializable |
`-------------*/

#  include <cryptography/Exception.hh>
#  include <cryptography/rsa/PublicKey.hh>
#  include <cryptography/rsa/PrivateKey.hh>

#  include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_STATIC_FORMAT(infinit::cryptography::rsa::KeyPair, 2);

ELLE_SERIALIZE_SPLIT(infinit::cryptography::rsa::KeyPair);

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::rsa::KeyPair,
                          archive,
                          value,
                          format)
{
  ELLE_ASSERT_NEQ(value._K, nullptr);
  ELLE_ASSERT_NEQ(value._k, nullptr);

  switch (format)
  {
    case 0:
    {
      archive << *value._K;
      archive << *value._k;

      break;
    }
    case 1:
    {
      archive << value._K;
      archive << value._k;

      break;
    }
    case 2:
    {
      archive << *value._K;
      archive << *value._k;

      break;
    }
    default:
      throw infinit::cryptography::Exception(
        elle::sprintf("unknown format '%s'", format));
  }
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::rsa::KeyPair,
                          archive,
                          value,
                          format)
{
  switch (format)
  {
    case 0:
    {
      value._K.reset(new infinit::cryptography::rsa::PublicKey(archive));
      value._k.reset(new infinit::cryptography::rsa::PrivateKey(archive));

      break;
    }
    case 1:
    {
      archive >> value._K;
      archive >> value._k;

      break;
    }
    case 2:
    {
      value._K.reset(new infinit::cryptography::rsa::PublicKey);
      value._k.reset(new infinit::cryptography::rsa::PrivateKey);

      archive >> *value._K;
      archive >> *value._k;

      break;
    }
    default:
      throw infinit::cryptography::Exception(
        elle::sprintf("unknown format '%s'", format));
  }

  ELLE_ASSERT_NEQ(value._K, nullptr);
  ELLE_ASSERT_NEQ(value._k, nullptr);
}

# endif

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
      // XXX return (std::hash<infinit::cryptography::rsa::PublicKey>()(value.K()));
      return (0);
    }
  };
}

#endif
