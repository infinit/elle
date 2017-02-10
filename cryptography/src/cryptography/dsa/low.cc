#include <cryptography/dsa/low.hh>
#include <cryptography/dsa/der.hh>
#include <cryptography/finally.hh>
#include <cryptography/Error.hh>

#include <elle/log.hh>
#include <elle/Buffer.hh>

#include <openssl/dsa.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/err.h>

namespace elle
{
  namespace cryptography
  {
    namespace dsa
    {
      namespace low
      {
        /*----------.
        | Functions |
        `----------*/

        // OpenSSL does not provide an easy way to duplicate/convert a private
        // key into a public key.
        ::DSA*
        DSA_priv2pub(::DSA* private_key)
        {
          ELLE_ASSERT_NEQ(private_key, nullptr);

          unsigned char* buffer = nullptr;
          int size = 0;

          if ((size = ::i2d_DSAPublicKey(private_key,
                                         &buffer)) <= 0)
            throw Error(
              elle::sprintf("unable to encode the DSA private key: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_OPENSSL(buffer);

          const unsigned char* _buffer = buffer;

          ::DSA* public_key = nullptr;
          if ((public_key = ::d2i_DSAPublicKey(NULL,
                                               &_buffer,
                                               size)) == NULL)
            throw Error(
              elle::sprintf("unable to decode the DSA private key: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_DSA(public_key);

          ELLE_CRYPTOGRAPHY_FINALLY_ABORT(buffer);
          ::OPENSSL_free(buffer);

          ELLE_CRYPTOGRAPHY_FINALLY_ABORT(public_key);

          return (public_key);
        }

        ::DSA*
        DSA_dup(::DSA* key)
        {
          ELLE_ASSERT_NEQ(key, nullptr);

          // Increase the reference counter on this object rather
          // than duplicating the structure.
          ::DSA_up_ref(key);

          return (key);
        }
      }
    }
  }
}
