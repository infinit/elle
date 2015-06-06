#ifndef INFINIT_CRYPTOGRAPHY_HMAC_HXX
# define INFINIT_CRYPTOGRAPHY_HMAC_HXX

# include <cryptography/Plain.hh>
# include <cryptography/evp.hh>
# include <cryptography/finally.hh>
# include <cryptography/serialization.hh>

# include <elle/serialize/BaseArchive.hxx>

# include <elle/Buffer.hh>
# include <elle/log.hh>
# include <elle/Measure.hh>

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

      template <typename T>
      Digest
      sign(T const& value,
           elle::String const& key,
           Oneway const oneway)
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.hmac");
        ELLE_TRACE_FUNCTION(key, oneway);
        ELLE_DUMP("value: %x", value);

        elle::Buffer _value = cryptography::serialize(value);

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
        elle::Buffer output = evp::hmac::sign(_value, _key, function);

        ::EVP_PKEY_free(_key);
        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(_key);

        return (Digest(output));
      }

      template <typename T,
                typename K>
      Digest
      sign(T const& value,
           K const& key,
           Oneway const oneway)
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.hmac");
        ELLE_TRACE_FUNCTION(oneway);
        ELLE_DUMP("value: %x", value);
        ELLE_DUMP("key: %x", key);

        elle::Buffer _value = cryptography::serialize(value);

        ::EVP_MD const* function = oneway::resolve(oneway);

        // Apply the HMAC function with the given key.
        elle::Buffer output =
          evp::hmac::sign(_value, key.key().get(), function);

        return (Digest(output));
      }

      template <typename T>
      elle::Boolean
      verify(Digest const& digest,
             T const& value,
             elle::String const& key,
             Oneway const oneway)
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.hmac");
        ELLE_TRACE_FUNCTION(key, oneway);
        ELLE_DUMP("digest: %x", digest);
        ELLE_DUMP("value: %x", value);

        elle::Buffer _value = cryptography::serialize(value);

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
        elle::Buffer output = evp::hmac::sign(_value, _key, function);

        ::EVP_PKEY_free(_key);
        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(_key);

        if (digest.buffer().size() != output.size())
          return (false);

        // Compare using low-level OpenSSL functions to prevent timing attacks.
        if (CRYPTO_memcmp(digest.buffer().contents(),
                          output.contents(),
                          output.size()) != 0)
          return (false);

        return (true);
      }

      template <typename T,
                typename K>
      elle::Boolean
      verify(Digest const& digest,
             T const& value,
             K const& key,
             Oneway const oneway)
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.hmac");
        ELLE_TRACE_FUNCTION(oneway);
        ELLE_DUMP("digest: %x", digest);
        ELLE_DUMP("value: %x", value);
        ELLE_DUMP("key: %x", key);

        elle::Buffer _value = cryptography::serialize(value);

        ::EVP_MD const* function = oneway::resolve(oneway);

        return (evp::hmac::verify(digest.buffer(),
                                  _value,
                                  key.key().get(),
                                  function));
      }
    }
  }
}

#endif
