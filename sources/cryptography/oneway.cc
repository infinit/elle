#include <cryptography/oneway.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/finally.hh>

#include <elle/log.hh>

#include <openssl/err.h>

ELLE_LOG_COMPONENT("infinit.cryptograhy.oneway");

namespace infinit
{
  namespace cryptography
  {
    namespace oneway
    {
      /*---------------.
      | Static Methods |
      `---------------*/

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
            throw elle::Exception("unable to resolve the given one-way "
                                  "function name '%s'", name);
          }

        elle::unreachable();
      }

      Digest
      hash(Plain const& plain,
           Algorithm algorithm)
      {
        ELLE_TRACE_FUNCTION(plain, algorithm);

        ::EVP_MD const* function = resolve(algorithm);
        Digest digest(EVP_MD_size(function));

        // Initialise the context.
        ::EVP_MD_CTX context;

        ::EVP_MD_CTX_init(&context);

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_CLEANUP_DIGEST_CONTEXT(context);

        // Initialise the digest.
        if (::EVP_DigestInit_ex(&context, function, nullptr) <= 0)
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

        ELLE_ASSERT(plain.buffer().contents() != nullptr);

        // Update the digest with the given plain's data.
        if (::EVP_DigestUpdate(&context,
                               plain.buffer().contents(),
                               plain.buffer().size()) <= 0)
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

        // Finalise the digest.
        unsigned int size;

        if (::EVP_DigestFinal_ex(&context,
                                 digest.buffer().mutable_contents(),
                                 &size) <=0)
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

        // Update the digest final size.
        digest.buffer().size(size);

        // Clean the context.
        if (::EVP_MD_CTX_cleanup(&context) <= 0)
          throw elle::Exception("unable to clean the context: %s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(context);

        return (digest);
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
            throw elle::Exception("unknown one-way algorithm '%s'",
                                  static_cast<int>(algorithm));
          }

        return (stream);
      }
    }
  }
}
