#include <cryptography/rsa/low.hh>
#include <cryptography/rsa/der.hh>
#include <cryptography/finally.hh>

#include <elle/log.hh>
#include <elle/Buffer.hh>

#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/err.h>

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace low
      {
        /*----------.
        | Functions |
        `----------*/

        ::RSA*
        RSA_priv2pub(::RSA* private_key)
        {
          ELLE_ASSERT_NEQ(private_key, nullptr);

          return (::RSAPublicKey_dup(private_key));
        }

        ::RSA*
        RSA_dup(::RSA* key)
        {
          ELLE_ASSERT_NEQ(key, nullptr);

          // Increase the reference counter on this object rather
          // than duplicating the structure.
          ::RSA_up_ref(key);

          return (key);
        }
      }
    }
  }
}
