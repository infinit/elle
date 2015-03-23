#ifndef INFINIT_CRYPTOGRAPHY_RSA_PUBLICKEY_HXX
# define INFINIT_CRYPTOGRAPHY_RSA_PUBLICKEY_HXX

//
// ---------- Class -----------------------------------------------------------
//

/*-------------.
| Serializable |
`-------------*/

# include <elle/serialize/Serializer.hh>
# include <elle/serialize/StaticFormat.hh>

# include <cryptography/Exception.hh>
# include <cryptography/finally.hh>
# include <cryptography/bn.hh>

# include <openssl/rsa.h>
# include <openssl/x509.h>

ELLE_SERIALIZE_STATIC_FORMAT(infinit::cryptography::rsa::PublicKey, 2);

ELLE_SERIALIZE_SPLIT(infinit::cryptography::rsa::PublicKey)

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::rsa::PublicKey,
                          archive,
                          value,
                          format)
{
  ELLE_ASSERT_NEQ(value._key, nullptr);

  switch (format)
  {
    case 0:
    case 1:
    {
      archive << *value._key->pkey.rsa->n
              << *value._key->pkey.rsa->e;

      break;
    }
    case 2:
    {
      unsigned char* _buffer = nullptr;
      int _size = ::i2d_RSAPublicKey(value._key->pkey.rsa, &_buffer);
      if (_size <= 0)
        throw infinit::cryptography::Exception(
          elle::sprintf("unable to encode the RSA public key: %s",
                        ::ERR_error_string(ERR_get_error(), nullptr)));

      // XXX[use a WeakBuffer to avoid a copy]
      //elle::ConstWeakBuffer buffer(_buffer, _size);
      //archive << buffer;
      // XXX
      elle::Buffer buffer(_buffer, _size);
      archive << buffer;

      ::OPENSSL_free(_buffer);

      break;
    }
    default:
      throw infinit::cryptography::Exception(
        elle::sprintf("unknown format '%s'", format));
  }
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::rsa::PublicKey,
                          archive,
                          value,
                          format)
{
  switch (format)
  {
    case 0:
    case 1:
    {
      ::BIGNUM *n = ::BN_new();
      ::BIGNUM *e = ::BN_new();

      INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(n);
      INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(e);

      ELLE_ASSERT_NEQ(n, nullptr);
      ELLE_ASSERT_NEQ(e, nullptr);

      archive >> *n
              >> *e;

      value._construct(n, e);

      INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(n);
      INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(e);

      break;
    }
    case 2:
    {
      elle::Buffer buffer;
      archive >> buffer;

      const unsigned char* _buffer = buffer.contents();
      long _size = buffer.size();

      ::RSA* rsa = nullptr;
      if ((rsa = ::d2i_RSAPublicKey(NULL, &_buffer, _size)) == NULL)
        throw infinit::cryptography::Exception(
          elle::sprintf("unable to decode the RSA public key: %s",
                        ::ERR_error_string(ERR_get_error(), nullptr)));

      value._construct(rsa);

      break;
    }
    default:
      throw infinit::cryptography::Exception(
        elle::sprintf("unknown format '%s'", format));
  }

  value._prepare();

  ELLE_ASSERT_NEQ(value._key, nullptr);
}

#endif
