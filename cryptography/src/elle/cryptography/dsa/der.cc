#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/dsa.h>

#include <elle/cryptography/dsa/der.hh>
#include <elle/cryptography/Error.hh>
#include <elle/cryptography/finally.hh>

namespace elle
{
  namespace cryptography
  {
    namespace dsa
    {
      namespace der
      {
        /*----------.
        | Functions |
        `----------*/

        elle::Buffer
        encode_public(::DSA* dsa)
        {
          unsigned char* _buffer = nullptr;

          int _size = ::i2d_DSAPublicKey(dsa, &_buffer);
          if (_size <= 0)
            throw Error(
              elle::sprintf("unable to encode DER for the DSA public key: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_OPENSSL(_buffer);

          elle::Buffer buffer(_buffer, _size);

          ELLE_CRYPTOGRAPHY_FINALLY_ABORT(_buffer);
          ::OPENSSL_free(_buffer);

          return (buffer);
        }

        ::DSA*
        decode_public(elle::ConstWeakBuffer const& buffer)
        {
          const unsigned char* _buffer = buffer.contents();
          long _size = buffer.size();

          ::DSA* dsa = nullptr;
          if ((dsa = ::d2i_DSAPublicKey(NULL, &_buffer, _size)) == NULL)
            throw Error(
              elle::sprintf("unable to decode DER for the DSA public key: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          return (dsa);
        }

        elle::Buffer
        encode_private(::DSA* dsa)
        {
          unsigned char* _buffer = nullptr;

          int _size = ::i2d_DSAPrivateKey(dsa, &_buffer);
          if (_size <= 0)
            throw Error(
              elle::sprintf("unable to encode the DSA private key: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_OPENSSL(_buffer);

          elle::Buffer buffer(_buffer, _size);

          ELLE_CRYPTOGRAPHY_FINALLY_ABORT(_buffer);
          ::OPENSSL_free(_buffer);

          return (buffer);
        }

        ::DSA*
        decode_private(elle::ConstWeakBuffer const& buffer)
        {
          const unsigned char* _buffer = buffer.contents();
          long _size = buffer.size();

          ::DSA* dsa = nullptr;
          if ((dsa = ::d2i_DSAPrivateKey(NULL, &_buffer, _size)) == NULL)
            throw Error(
              elle::sprintf("unable to decode the DSA private key: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          return (dsa);
        }
      }
    }
  }
}
