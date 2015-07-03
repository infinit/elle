#ifndef INFINIT_CRYPTOGRAPHY_HMAC_HXX
# define INFINIT_CRYPTOGRAPHY_HMAC_HXX

# include <cryptography/Plain.hh>
# include <cryptography/evp.hh>
# include <cryptography/finally.hh>
# include <cryptography/Exception.hh>
# include <cryptography/serialization.hh>

# include <elle/Buffer.hh>
# include <elle/log.hh>

# include <utility>

# include <openssl/evp.h>
# include <openssl/err.h>
# include <openssl/hmac.h>

namespace infinit
{
  namespace cryptography
  {
    namespace hmac
    {
      /*----------.
      | Functions |
      `----------*/

      template <typename K>
      Digest
      sign(Plain const& plain,
           K const& key,
           Oneway const oneway)
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.hmac");
        ELLE_TRACE_FUNCTION(oneway);
        ELLE_DUMP("plain: %x", plain);
        ELLE_DUMP("key: %x", key);

        ::EVP_MD const* function = oneway::resolve(oneway);

        // Apply the HMAC function with the given key.
        elle::Buffer output =
          evp::hmac::sign(plain.buffer(), key.key().get(), function);

        return (Digest(output));
      }

      template <typename K>
      elle::Boolean
      verify(Digest const& digest,
             Plain const& plain,
             K const& key,
             Oneway const oneway)
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.hmac");
        ELLE_TRACE_FUNCTION(oneway);
        ELLE_DUMP("digest: %x", digest);
        ELLE_DUMP("plain: %x", plain);
        ELLE_DUMP("key: %x", key);

        ::EVP_MD const* function = oneway::resolve(oneway);

        return (evp::hmac::verify(digest.buffer(),
                                  plain.buffer(),
                                  key.key().get(),
                                  function));
      }
    }
  }
}

#endif
