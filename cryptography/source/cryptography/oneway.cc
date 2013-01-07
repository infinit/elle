#include <cryptography/oneway.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/finally.hh>

#include <elle/log.hh>

#include <openssl/evp.h>
#include <openssl/err.h>

ELLE_LOG_COMPONENT("infinit.cryptograhy.oneway");

/*----------------.
| Macro-functions |
`----------------*/

#define INFINIT_CRYPTOGRAPHY_ONEWAY_ALGORITHM(_name_)                    \
  case infinit::cryptography::oneway::Algorithm::_name_:                \
    return (::EVP_ ## _name_());

namespace infinit
{
  namespace cryptography
  {
    namespace oneway
    {
      /*-----------------.
      | Static Functions |
      `-----------------*/

      /// Resolve a algorithm name into an EVP function pointer.
      static
      ::EVP_MD const*
      resolve(Algorithm const name)
      {
        switch (name)
          {
            INFINIT_CRYPTOGRAPHY_ONEWAY_ALGORITHM(md5);
            INFINIT_CRYPTOGRAPHY_ONEWAY_ALGORITHM(sha);
            INFINIT_CRYPTOGRAPHY_ONEWAY_ALGORITHM(sha1);
            INFINIT_CRYPTOGRAPHY_ONEWAY_ALGORITHM(sha224);
            INFINIT_CRYPTOGRAPHY_ONEWAY_ALGORITHM(sha256);
            INFINIT_CRYPTOGRAPHY_ONEWAY_ALGORITHM(sha384);
            INFINIT_CRYPTOGRAPHY_ONEWAY_ALGORITHM(sha512);
          default:
            throw elle::Exception("unable to resolve the given one-way "
                                  "function name");
          }

        elle::unreachable();
      }

      /*---------------.
      | Static Methods |
      `---------------*/

      // Note that the value return is not an rvalue in order to benefit from
      // the 'return value optimization'.
      //
      // The caller could very well 'std::move' the result if required.
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
    }
  }
}
