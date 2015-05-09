#include <cryptography/hmac.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/finally.hh>
#include <cryptography/evp.hh>
#include <cryptography/Exception.hh>
#include <cryptography/Plain.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/types.hh>

#include <elle/log.hh>

#include <openssl/err.h>

ELLE_LOG_COMPONENT("infinit.cryptography.hmac");

namespace infinit
{
  namespace cryptography
  {
    namespace hmac
    {
      /*-----------------.
      | Static Functions |
      `-----------------*/

      static
      elle::Buffer
      _apply(elle::ConstWeakBuffer const& input,
             elle::String const& key,
             Oneway const oneway)
      {
        ELLE_TRACE_FUNCTION(key, oneway);
        ELLE_DUMP("input: %x", input);

        ::EVP_MD const* function = oneway::resolve(oneway);

        ::EVP_PKEY *_key;

        if ((_key = ::EVP_PKEY_new_mac_key(EVP_PKEY_HMAC,
                                           NULL,
                                           (const unsigned char*)key.data(),
                                           key.size())) == nullptr)
          throw Exception(
            elle::sprintf("unable to generate a MAC key: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(_key);

        // Apply the HMAC function with the given key.
        elle::Buffer output = evp::digest::hmac(input, _key, function);

        ::EVP_PKEY_free(_key);
        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(_key);

        return (output);
      }

      /*----------.
      | Functions |
      `----------*/

      Digest
      sign(Plain const& plain,
           elle::String const& key,
           Oneway const oneway)
      {
        ELLE_TRACE_FUNCTION(key, oneway);
        ELLE_DUMP("plain: %x", plain);

        // Apply the HMAC function.
        return (Digest(_apply(plain.buffer(), key, oneway)));
      }

      elle::Boolean
      verify(Digest const& digest,
             Plain const& plain,
             elle::String const& key,
             Oneway const oneway)
      {
        ELLE_TRACE_FUNCTION(key, oneway);
        ELLE_DUMP("digest: %x", digest);
        ELLE_DUMP("plain: %x", plain);

        // Recompute the HMAC.
        elle::Buffer const output = _apply(plain.buffer(), key, oneway);

        if (digest.buffer().size() != output.size())
          return (false);

        // Compare using low-level OpenSSL functions to prevent timing attacks.
        if (CRYPTO_memcmp(digest.buffer().contents(),
                          output.contents(),
                          output.size()) != 0)
          return (false);

        return (true);
      }
    }
  }
}
