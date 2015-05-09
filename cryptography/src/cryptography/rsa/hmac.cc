#include <cryptography/rsa/hmac.hh>
#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/finally.hh>
#include <cryptography/evp.hh>
#include <cryptography/Exception.hh>
#include <cryptography/Plain.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/types.hh>

#include <elle/log.hh>

#include <openssl/err.h>

ELLE_LOG_COMPONENT("infinit.cryptography.rsa.hmac");

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace hmac
      {
        /*----------.
        | Functions |
        `----------*/

        Digest
        sign(Plain const& plain,
             PrivateKey const& k,
             Oneway const oneway)
        {
          ELLE_TRACE_FUNCTION(k, oneway);
          ELLE_DUMP("plain: %x", plain);

          ::EVP_MD const* function = oneway::resolve(oneway);

          // Apply the HMAC function with the given key.
          return (Digest(evp::hmac::sign(plain.buffer(),
                                         k.key().get(),
                                         function)));
        }

        elle::Boolean
        verify(Digest const& digest,
               Plain const& plain,
               PublicKey const& K,
               Oneway const oneway)
        {
          ELLE_TRACE_FUNCTION(K, oneway);
          ELLE_DUMP("digest: %x", digest);
          ELLE_DUMP("plain: %x", plain);

          ::EVP_MD const* function = oneway::resolve(oneway);

          // Apply the HMAC verification function with the opposite key.
          return (evp::hmac::verify(digest.buffer(),
                                    plain.buffer(),
                                    K.key().get(),
                                    function));
        }
      }
    }
  }
}
