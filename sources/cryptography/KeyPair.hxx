#ifndef INFINIT_CRYPTOGRAPHY_KEYPAIR_HXX
# define INFINIT_CRYPTOGRAPHY_KEYPAIR_HXX

/*-------------.
| Serializable |
`-------------*/

# include <cryptography/PublicKey.hh>
# include <cryptography/PrivateKey.hh>
# include <cryptography/Exception.hh>

# include <elle/serialize/Serializer.hh>
# include <elle/serialize/StaticFormat.hh>

ELLE_SERIALIZE_STATIC_FORMAT(infinit::cryptography::KeyPair, 1);

ELLE_SERIALIZE_SPLIT(infinit::cryptography::KeyPair);

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::KeyPair,
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
    default:
      // XXX ::elle::cryptography::Exception
      throw infinit::cryptography::Exception(
        elle::sprintf("unknown format '%s'", format));
  }
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::KeyPair,
                          archive,
                          value,
                          format)
{
  switch (format)
  {
    case 0:
    {
      value._K.reset(new infinit::cryptography::PublicKey(archive));
      value._k.reset(new infinit::cryptography::PrivateKey(archive));

      break;
    }
    case 1:
    {
      archive >> value._K;
      archive >> value._k;

      break;
    }
    default:
      // XXX ::elle::cryptography::Exception
      throw infinit::cryptography::Exception(
        elle::sprintf("unknown format '%s'", format));
  }

  ELLE_ASSERT_NEQ(value._K, nullptr);
  ELLE_ASSERT_NEQ(value._k, nullptr);
}

#endif
