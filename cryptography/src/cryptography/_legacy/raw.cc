#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
# include <cryptography/_legacy/raw.hh>
# include <cryptography/cryptography.hh>
# include <cryptography/SecretKey.hh>
# include <cryptography/Cipher.hh>
# include <cryptography/Oneway.hh>
# include <cryptography/finally.hh>
# include <cryptography/Error.hh>

# include <elle/Buffer.hh>
# include <elle/log.hh>

# include <openssl/crypto.h>
# include <openssl/engine.h>
# include <openssl/err.h>
# include <openssl/hmac.h>
# include <openssl/rand.h>

ELLE_LOG_COMPONENT("infinit.cryptography.legacy.raw");

//
// ---------- Asymmetric ------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace raw
    {
      /*-----------------.
      | Static Functions |
      `-----------------*/

      /// Normalize behavior between no data and data of size 0.
      /// by always forcing it into the second
      static
      elle::ConstWeakBuffer
      _normalize(elle::ConstWeakBuffer const& input)
      {
        static elle::ConstWeakBuffer blank("", 0);

        if (input.contents() != nullptr)
          return (input);

        return (blank);
      }

      namespace asymmetric
      {
        /*-----------------.
        | Static Functions |
        `-----------------*/

        static
        elle::Buffer
        _apply(elle::ConstWeakBuffer const& input,
               ::EVP_PKEY_CTX* context,
               int (*function)(EVP_PKEY_CTX*,
                               unsigned char*,
                               size_t*,
                               const unsigned char*,
                               size_t))
        {
          ELLE_DEBUG_FUNCTION(context, function);
          ELLE_DUMP("input: %s", input);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          ELLE_ASSERT_NEQ(context, nullptr);
          ELLE_ASSERT_GTE(static_cast<elle::Natural32>(
                            ::EVP_PKEY_bits(::EVP_PKEY_CTX_get0_pkey(context))),
                          input.size() * 8);

          // Normalize the input.
          elle::ConstWeakBuffer const _input = _normalize(input);

          // Compute the size of the encrypted buffer.
          ::size_t size;

          if (function(context,
                       nullptr,
                       &size,
                       _input.contents(),
                       _input.size()) <= 0)
            throw Error(elle::sprintf("unable to pre-compute the size of "
                                      "the encrypted output: %s",
                                      ::ERR_error_string(ERR_get_error(),
                                                         nullptr)));

          // Prepare the output buffer for receiving the encrypted content.
          elle::Buffer output(size);

          // Encrypt the input buffer.
          if (function(context,
                       reinterpret_cast<unsigned char*>(
                         output.mutable_contents()),
                       &size,
                       _input.contents(),
                       _input.size()) <= 0)
            throw Error(elle::sprintf("unable to apply the cryptographic "
                                      "function: %s",
                                      ::ERR_error_string(ERR_get_error(),
                                                         nullptr)));

          // Set the final output buffer size.
          output.size(size);
          output.shrink_to_fit();

          return (output);
        }

        /*----------.
        | Functions |
        `----------*/

        elle::Buffer
        sign(elle::ConstWeakBuffer const& digest,
             ::EVP_PKEY_CTX* context,
             int (*function)(EVP_PKEY_CTX*,
                             unsigned char*,
                             size_t*,
                             const unsigned char*,
                             size_t))
        {
          ELLE_DEBUG_FUNCTION(context, function);
          ELLE_DUMP("digest: %s", digest);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          return (_apply(digest, context, function));
        }

        elle::Boolean
        verify(elle::ConstWeakBuffer const& signature,
               elle::ConstWeakBuffer const& digest,
               ::EVP_PKEY_CTX* context,
               int (*function)(EVP_PKEY_CTX*,
                               const unsigned char*,
                               size_t,
                               const unsigned char*,
                               size_t))
        {
          ELLE_DEBUG_FUNCTION(context, function);
          ELLE_DUMP("signature: %s", signature);
          ELLE_DUMP("digest: %s", digest);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          ELLE_ASSERT_NEQ(context, nullptr);
          ELLE_ASSERT_NEQ(signature.contents(), nullptr);
          ELLE_ASSERT_NEQ(digest.contents(), nullptr);

          // Verify the signature.
          int result =
            function(context,
                     reinterpret_cast<const unsigned char*>(
                       signature.contents()),
                     signature.size(),
                     reinterpret_cast<const unsigned char*>(
                       digest.contents()),
                     digest.size());

          switch (result)
          {
            case 1:
              return (true);
            case 0:
              return (false);
            default:
              throw Error(
                elle::sprintf("unable to verify the signature: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));
          }

          elle::unreachable();
        }
      }
    }
  }
}

#endif
