#include <elle/cryptography/oneway.hh>
#include <elle/cryptography/cryptography.hh>

#include <openssl/evp.h>
#include <openssl/err.h>

/*----------------.
| Macro-functions |
`----------------*/

#define ELLE_CRYPTOGRAPHY_ONEWAY_FUNCTION(_name_)                       \
  case elle::cryptography::oneway::Algorithm::_name_:                   \
    return (::EVP_ ## _name_());

#define ELLE_CRYPTOGRAPHY_ONEWAY_RETURN(_message_)                      \
  do                                                                    \
    {                                                                   \
      ::EVP_MD_CTX_cleanup(&context);                                   \
      throw Exception(_message_ " %s",                                  \
                      ::ERR_error_string(ERR_get_error(), nullptr));    \
    } while (0)

namespace elle
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
            ELLE_CRYPTOGRAPHY_ONEWAY_FUNCTION(md5);
            ELLE_CRYPTOGRAPHY_ONEWAY_FUNCTION(sha);
            ELLE_CRYPTOGRAPHY_ONEWAY_FUNCTION(sha1);
            ELLE_CRYPTOGRAPHY_ONEWAY_FUNCTION(sha224);
            ELLE_CRYPTOGRAPHY_ONEWAY_FUNCTION(sha256);
            ELLE_CRYPTOGRAPHY_ONEWAY_FUNCTION(sha384);
            ELLE_CRYPTOGRAPHY_ONEWAY_FUNCTION(sha512);
          default:
              throw Exception("unable to resolve the given one-way "
                              "function name");
          }

        elle::unreachable();
      }

      /*---------------.
      | Static Methods |
      `---------------*/

      Digest
      hash(Plain const& plain,
           Algorithm algorithm)
      {
        ::EVP_MD const* function = resolve(algorithm);

        Digest digest(EVP_MD_size(function));
        ::EVP_MD_CTX context;
        unsigned int size;

        // Initialise the context.
        ::EVP_MD_CTX_init(&context);

        // Initialise the digest.
        if (::EVP_DigestInit_ex(&context, function, nullptr) <= 0)
          ELLE_CRYPTOGRAPHY_ONEWAY_RETURN("unable to initialize the context");

        // Update the digest with the given plain's data.
        if (::EVP_DigestUpdate(&context,
                               plain.buffer().contents(),
                               plain.buffer().size()) <= 0)
          ELLE_CRYPTOGRAPHY_ONEWAY_RETURN("unable to update the context");

        // Finalise the digest.
        if (::EVP_DigestFinal_ex(&context,
                                 digest.buffer().mutable_contents(),
                                 &size) <=0)
          ELLE_CRYPTOGRAPHY_ONEWAY_RETURN("unable to finalize the context");

        // Update the digest final size.
        digest.buffer().size(size);

        // Clean the context.
        if (::EVP_MD_CTX_cleanup(&context) <= 0)
          throw Exception("unable to clean the context: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr));

        return (digest);
      }
    }
  }
}
