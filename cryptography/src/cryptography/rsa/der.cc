#include <cryptography/Error.hh>
#include <cryptography/finally.hh>
#include <cryptography/rsa/der.hh>

#include <openssl/err.h>
#include <openssl/x509.h>

namespace elle
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace der
      {
        /*----------.
        | Functions |
        `----------*/

        elle::Buffer
        encode_public(::RSA* rsa)
        {
          unsigned char* _buffer = nullptr;

          int _size = ::i2d_RSAPublicKey(rsa, &_buffer);
          if (_size <= 0)
            throw Error(
              elle::sprintf("unable to encode DER for RSA public key: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_OPENSSL(_buffer);

          elle::Buffer buffer(_buffer, _size);

          ELLE_CRYPTOGRAPHY_FINALLY_ABORT(_buffer);
          ::OPENSSL_free(_buffer);

          return (buffer);
        }

        ::RSA*
        decode_public(elle::ConstWeakBuffer const& buffer)
        {
          const unsigned char* _buffer = buffer.contents();
          long _size = buffer.size();

          ::RSA* rsa = nullptr;
          if ((rsa = ::d2i_RSAPublicKey(NULL, &_buffer, _size)) == NULL)
            throw Error(
              elle::sprintf("unable to decode DER for RSA public key: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          return (rsa);
        }

        elle::Buffer
        encode_private(::RSA* rsa)
        {
          unsigned char* _buffer = nullptr;

          int _size = ::i2d_RSAPrivateKey(rsa, &_buffer);
          if (_size <= 0)
            throw Error(
              elle::sprintf("unable to encode the RSA private key: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_OPENSSL(_buffer);

          elle::Buffer buffer(_buffer, _size);

          ELLE_CRYPTOGRAPHY_FINALLY_ABORT(_buffer);
          ::OPENSSL_free(_buffer);

          return (buffer);
        }

        ::RSA*
        decode_private(elle::ConstWeakBuffer const& buffer)
        {
          const unsigned char* _buffer = buffer.contents();
          long _size = buffer.size();

          ::RSA* rsa = nullptr;
          if ((rsa = ::d2i_RSAPrivateKey(NULL, &_buffer, _size)) == NULL)
            throw Error(
              elle::sprintf("unable to decode the RSA private key: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          return (rsa);
        }
      }
    }
  }
}
