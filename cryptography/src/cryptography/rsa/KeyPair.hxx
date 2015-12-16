#ifndef INFINIT_CRYPTOGRAPHY_RSA_KEYPAIR_HXX
# define INFINIT_CRYPTOGRAPHY_RSA_KEYPAIR_HXX

//
// ---------- Class -----------------------------------------------------------
//

# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)

/*-------------.
| Serializable |
`-------------*/

#  include <cryptography/Error.hh>
#  include <cryptography/rsa/PublicKey.hh>
#  include <cryptography/rsa/PrivateKey.hh>

#  include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_STATIC_FORMAT(infinit::cryptography::rsa::KeyPair, 1);

ELLE_SERIALIZE_SPLIT(infinit::cryptography::rsa::KeyPair);

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::rsa::KeyPair,
                          archive,
                          value,
                          format)
{
  ELLE_ASSERT_NEQ(value._public_key, nullptr);
  ELLE_ASSERT_NEQ(value._private_key, nullptr);

  switch (format)
  {
    case 0:
    {
      archive << *value._public_key;
      archive << *value._private_key;

      break;
    }
    case 1:
    {
      archive << value._public_key;
      archive << value._private_key;

      break;
    }
    default:
      throw infinit::cryptography::Error(
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
      value._public_key.reset(new infinit::cryptography::rsa::PublicKey(archive));
      value._private_key.reset(new infinit::cryptography::rsa::PrivateKey(archive));

      break;
    }
    case 1:
    {
      archive >> value._public_key;
      archive >> value._private_key;

      break;
    }
    default:
      throw infinit::cryptography::Error(
        elle::sprintf("unknown format '%s'", format));
  }

  ELLE_ASSERT_NEQ(value._public_key, nullptr);
  ELLE_ASSERT_NEQ(value._private_key, nullptr);
}

# endif

//
// ---------- Hash ------------------------------------------------------------
//

# include <cryptography/rsa/PublicKey.hh>

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
