#ifndef INFINIT_CRYPTOGRAPHY_HMAC_HXX
# define INFINIT_CRYPTOGRAPHY_HMAC_HXX

# include <cryptography/raw.hh>
# include <cryptography/finally.hh>
# include <cryptography/Exception.hh>

# include <elle/Buffer.hh>
# include <elle/log.hh>

# include <utility>

# include <openssl/evp.h>
# include <openssl/err.h>
# include <openssl/hmac.h>

# if !defined(INFINIT_CRYPTOGRAPHY_LEGACY)
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
      elle::Buffer
      sign(elle::ConstWeakBuffer const& plain,
           K const& key,
           Oneway const oneway)
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.hmac");
        ELLE_TRACE_FUNCTION(oneway);
        ELLE_DUMP("key: %x", key);

        elle::IOStream _plain(plain.istreambuf());

        return (sign(_plain, key, oneway));
      }

      template <typename K>
      elle::Boolean
      verify(elle::ConstWeakBuffer const& digest,
             elle::ConstWeakBuffer const& plain,
             K const& key,
             Oneway const oneway)
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.hmac");
        ELLE_TRACE_FUNCTION(oneway);
        ELLE_DUMP("key: %x", key);

        elle::IOStream _plain(plain.istreambuf());

        return (verify(digest, _plain, key, oneway));
      }

      template <typename K>
      elle::Buffer
      sign(std::istream& plain,
           K const& key,
           Oneway const oneway)
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.hmac");
        ELLE_TRACE_FUNCTION(oneway);
        ELLE_DUMP("key: %x", key);

        ::EVP_MD const* function = oneway::resolve(oneway);

        // Apply the HMAC function with the given key.
        elle::Buffer digest =
          raw::hmac::sign(plain, key.key().get(), function);

        return (digest);
      }

      template <typename K>
      elle::Boolean
      verify(elle::ConstWeakBuffer const& digest,
             std::istream& plain,
             K const& key,
             Oneway const oneway)
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.hmac");
        ELLE_TRACE_FUNCTION(oneway);
        ELLE_DUMP("key: %x", key);

        ::EVP_MD const* function = oneway::resolve(oneway);

        return (raw::hmac::verify(digest,
                                  plain,
                                  key.key().get(),
                                  function));
      }
    }
  }
}
# else
#  include <cryptography/_legacy/Digest.hh>
#  include <cryptography/_legacy/serialization.hh>

namespace infinit
{
  namespace cryptography
  {
    namespace hmac
    {
      template <typename T>
      Digest
      sign(T const& value,
           elle::ConstWeakBuffer const& key,
           Oneway const oneway)
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.hmac");
        ELLE_TRACE_FUNCTION(oneway);
        ELLE_DUMP("value: %x", value);

        elle::Buffer archive = cryptography::serialize(value);

        elle::IOStream _archive(archive.istreambuf());

        return (Digest(sign(static_cast<std::istream&>(_archive),
                            key.string(),
                            oneway)));
      }
    }
  }
}
# endif

#endif
