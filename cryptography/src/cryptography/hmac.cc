#include <cryptography/hmac.hh>
#include <cryptography/evp.hh>
#include <cryptography/Plain.hh>
#include <cryptography/Digest.hh>

#include <elle/Buffer.hh>
#include <elle/log.hh>

namespace infinit
{
  namespace cryptography
  {
    namespace hmac
    {
      /*----------.
      | Functions |
      `----------*/

#if !defined(INFINIT_CRYPTOGRAPHY_LEGACY)
      Digest
      sign(Plain const& plain,
           elle::String const& key,
           Oneway const oneway)
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.hmac");
        ELLE_TRACE_FUNCTION(key, oneway);
        ELLE_DUMP("plain: %x", plain);

        ::EVP_MD const* function = oneway::resolve(oneway);

        ::EVP_PKEY *_key = nullptr;

        if ((_key = ::EVP_PKEY_new_mac_key(EVP_PKEY_HMAC,
                                           NULL,
                                           (const unsigned char*)key.data(),
                                           key.size())) == nullptr)
          throw Exception(
            elle::sprintf("unable to generate a MAC key: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(_key);

        // Apply the HMAC function with the given key.
        elle::Buffer output = evp::hmac::sign(plain.buffer(), _key, function);

        ::EVP_PKEY_free(_key);
        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(_key);

        return (Digest(output));
      }

      elle::Boolean
      verify(Digest const& digest,
             Plain const& plain,
             elle::String const& key,
             Oneway const oneway)
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.hmac");
        ELLE_TRACE_FUNCTION(key, oneway);
        ELLE_DUMP("digest: %x", digest);
        ELLE_DUMP("plain: %x", plain);

        Digest _digest = sign(plain, key, oneway);

        if (digest.buffer().size() != _digest.buffer().size())
          return (false);

        // Compare using low-level OpenSSL functions to prevent timing attacks.
        if (CRYPTO_memcmp(digest.buffer().contents(),
                          _digest.buffer().contents(),
                          _digest.buffer().size()) != 0)
          return (false);

        return (true);
      }
#endif
    }
  }
}
