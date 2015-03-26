#include <cryptography/oneway.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/finally.hh>
#include <cryptography/evp.hh>
#include <cryptography/Exception.hh>

#include <elle/log.hh>

#include <openssl/err.h>

ELLE_LOG_COMPONENT("infinit.cryptography.oneway");

namespace infinit
{
  namespace cryptography
  {
    namespace oneway
    {
      /*----------.
      | Functions |
      `----------*/

      ::EVP_MD const*
      resolve(Algorithm const name)
      {
        ELLE_TRACE_FUNCTION(name);

        switch (name)
        {
          case Algorithm::md5:
            return (::EVP_md5());
          case Algorithm::sha:
            return (::EVP_sha());
          case Algorithm::sha1:
            return (::EVP_sha1());
          case Algorithm::sha224:
            return (::EVP_sha224());
          case Algorithm::sha256:
            return (::EVP_sha256());
          case Algorithm::sha384:
            return (::EVP_sha384());
          case Algorithm::sha512:
            return (::EVP_sha512());
          default:
            throw Exception(elle::sprintf("unable to resolve the given one-way "
                                          "function name '%s'", name));
        }
        elle::unreachable();
      }

      Digest
      hash(Plain const& plain,
           Algorithm algorithm)
      {
        ELLE_TRACE_FUNCTION(algorithm);
        ELLE_DUMP("plain: %x", plain);

        ::EVP_MD const* function = resolve(algorithm);

        return (evp::digest::hash(plain, function));
      }

      Digest
      hmac(Plain const& plain,
           Digest const& key,
           Algorithm algorithm)
      {
        ELLE_TRACE_FUNCTION(algorithm);
        ELLE_DUMP("plain: %x", plain);
        ELLE_DUMP("digest: %x", key);

        ::EVP_MD const* function = resolve(algorithm);

        return (evp::digest::hmac(plain, key, function));
      }

      /*----------.
      | Operators |
      `----------*/

      std::ostream&
      operator <<(std::ostream& stream,
                  Algorithm const algorithm)
      {
        switch (algorithm)
        {
          case Algorithm::md5:
          {
            stream << "md5";
            break;
          }
          case Algorithm::sha:
          {
            stream << "sha";
            break;
          }
          case Algorithm::sha1:
          {
            stream << "sha1";
            break;
          }
          case Algorithm::sha224:
          {
            stream << "sha224";
            break;
          }
          case Algorithm::sha256:
          {
            stream << "sha256";
            break;
          }
          case Algorithm::sha384:
          {
            stream << "sha384";
            break;
          }
          case Algorithm::sha512:
          {
            stream << "sha512";
            break;
          }
          default:
            throw Exception(elle::sprintf("unknown one-way algorithm '%s'",
                                          static_cast<int>(algorithm)));
        }

        return (stream);
      }
    }
  }
}
