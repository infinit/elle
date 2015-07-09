#include <cryptography/evp.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/SecretKey.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/finally.hh>
#include <cryptography/Exception.hh>

#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
# include <cryptography/_legacy/Plain.hh>
# include <cryptography/_legacy/Signature.hh>
#endif

#include <elle/Buffer.hh>
#include <elle/log.hh>
#include <elle/serialization/binary.hh>

#include <openssl/crypto.h>
#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>

ELLE_LOG_COMPONENT("infinit.cryptography.evp");

/*----------.
| Constants |
`----------*/

/// The size of the chunk to process iteratively from the streams.
static elle::Natural32 const chunk_size = 524288;

//
// ---------- Asymmetric ------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace evp
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
            throw Exception(elle::sprintf("unable to pre-compute the size of "
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
            throw Exception(elle::sprintf("unable to apply the cryptographic "
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
        encrypt(elle::ConstWeakBuffer const& plain,
                ::EVP_PKEY_CTX* context,
                int (*function)(EVP_PKEY_CTX*,
                                unsigned char*,
                                size_t*,
                                const unsigned char*,
                                size_t))
        {
          ELLE_DEBUG_FUNCTION(context, function);
          ELLE_DUMP("plain: %s", plain);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          return (_apply(plain, context, function));
        }

        elle::Buffer
        decrypt(elle::ConstWeakBuffer const& code,
                ::EVP_PKEY_CTX* context,
                int (*function)(EVP_PKEY_CTX*,
                                unsigned char*,
                                size_t*,
                                const unsigned char*,
                                size_t))
        {
          ELLE_DEBUG_FUNCTION(context, function);
          ELLE_DUMP("code: %s", code);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          return (_apply(code, context, function));
        }

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
              throw Exception(
                elle::sprintf("unable to verify the signature: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));
          }

          elle::unreachable();
        }

        elle::Buffer
        agree(::EVP_PKEY_CTX* context,
              ::EVP_PKEY* peer)
        {
          ELLE_DEBUG_FUNCTION(context, peer);

          // Set the peer key.
          if (::EVP_PKEY_derive_set_peer(context, peer) <= 0)
            throw Exception(
              elle::sprintf("unable to initialize the context for "
                            "derivation: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          size_t size;

          // Compute the shared key's future length.
          if (::EVP_PKEY_derive(context, nullptr, &size) <= 0)
            throw Exception(
              elle::sprintf("unable to compute the output size of the "
                            "shared key: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          elle::Buffer buffer(size);

          // Generate the shared key.
          if (::EVP_PKEY_derive(context,
                                buffer.mutable_contents(),
                                &size) <= 0)
            throw Exception(
              elle::sprintf("unable to generate the shared key: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Update the code size with the actual size of the generated data.
          buffer.size(size);
          buffer.shrink_to_fit();

          return (buffer);
        }

        elle::Buffer
        rotate(elle::ConstWeakBuffer const& seed,
               ::EVP_PKEY_CTX* context,
               int (*function)(EVP_PKEY_CTX*,
                               unsigned char*,
                               size_t*,
                               const unsigned char*,
                               size_t))
        {
          // Ensure the size of the seed equals the modulus.
          //
          // If the seed is too large, the algorithm would need to encrypt
          // it with a symmetric key etc. (as the seal() method does) which
          // would result in a future seed larger than the original.
          //
          // If it is too small, an attack could be performed against textbook
          // RSA, assuming it is the algorithm used in this case.
          if (::EVP_PKEY_size(::EVP_PKEY_CTX_get0_pkey(context)) !=
              seed.size())
            throw Exception(
              elle::sprintf("unable to rotate a seed whose length does not "
                            "match the key's modulus: %s versus %s",
                            ::EVP_PKEY_size(::EVP_PKEY_CTX_get0_pkey(context)),
                            seed.size()));

          elle::Buffer buffer =
            evp::asymmetric::_apply(seed, context, function);

          // Make sure the seed does not grow over time.
          ELLE_ASSERT_EQ(seed.size(), buffer.size());

          return (buffer);
        }

        elle::Buffer
        unrotate(elle::ConstWeakBuffer const& seed,
                 ::EVP_PKEY_CTX* context,
                 int (*function)(EVP_PKEY_CTX*,
                                 unsigned char*,
                                 size_t*,
                                 const unsigned char*,
                                 size_t))
        {
          // As for the rotation mechanism, ensure the size of the seed
          // equals the modulus.
          if (::EVP_PKEY_size(::EVP_PKEY_CTX_get0_pkey(context)) !=
              seed.size())
            throw Exception(
              elle::sprintf("unable to unrotate a seed whose length does not "
                            "match the key's modulus: %s versus %s",
                            ::EVP_PKEY_size(::EVP_PKEY_CTX_get0_pkey(context)),
                            seed.size()));

          elle::Buffer buffer =
            evp::asymmetric::_apply(seed, context, function);

          // Make sure the unrotated seed has the same size as the original.
          ELLE_ASSERT_EQ(seed.size(), buffer.size());

          return (buffer);
        }
      }
    }
  }
}

//
// ---------- Symmetric -------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace evp
    {
      namespace symmetric
      {
        /*----------.
        | Constants |
        `----------*/

        /// Define the magic which is embedded in every encrypted
        /// text so for the decryption process to know that the text
        /// has been salted.
        static elle::Character const magic[] = "Salted__";

        /*----------.
        | Functions |
        `----------*/

        void
        encipher(std::istream& plain,
                 std::ostream& code,
                 elle::ConstWeakBuffer const& secret,
                 ::EVP_CIPHER const* function_cipher,
                 ::EVP_MD const* function_oneway)
        {
          ELLE_DEBUG_FUNCTION(function_cipher, function_oneway);
          ELLE_DUMP("secret: %s", secret);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          // Generate a salt.
          unsigned char salt[PKCS5_SALT_LEN];

          if (::RAND_pseudo_bytes(salt, sizeof (salt)) <= 0)
            throw Exception(elle::sprintf("unable to pseudo-randomly generate "
                                          "a salt: %s",
                                          ::ERR_error_string(ERR_get_error(),
                                                             nullptr)));

          // Check that the secret key's buffer has a non-null address.
          //
          // Otherwise, EVP_BytesToKey() is non-deterministic :(
          ELLE_ASSERT_NEQ(secret.contents(), nullptr);

          // Generate a key/IV tuple based on the salt.
          unsigned char key[EVP_MAX_KEY_LENGTH];
          unsigned char iv[EVP_MAX_IV_LENGTH];

          if (::EVP_BytesToKey(function_cipher,
                               function_oneway,
                               salt,
                               secret.contents(),
                               secret.size(),
                               1,
                               key,
                               iv) > static_cast<int>(sizeof (key)))
            throw Exception("the generated key size is too large");

          // Initialize the cipher context.
          ::EVP_CIPHER_CTX context;

          ::EVP_CIPHER_CTX_init(&context);

          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_CLEANUP_CIPHER_CONTEXT(context);

          // Initialise the ciphering process.
          if (::EVP_EncryptInit_ex(&context,
                                   function_cipher,
                                   nullptr,
                                   key,
                                   iv) == 0)
            throw Exception(
              elle::sprintf("unable to initialize the encryption process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Embed the magic and salt directly into the output code.
          code.write(magic, sizeof (magic) - 1);
          code.write(reinterpret_cast<const char*>(salt), sizeof (salt));
          if (!code.good())
            throw Exception(
              elle::sprintf("unable to write the magic and salt to the code's "
                            "output stream: %s",
                            code.rdstate()));

          // Retreive the cipher-specific block size. This is the maximum size
          // that the algorithm can output on top of the encrypted input
          // plain text.
          int block_size = ::EVP_CIPHER_CTX_block_size(&context);
          ELLE_DEBUG("block size: %s", block_size);

          // Encrypt the input stream.
          std::vector<unsigned char> _input(chunk_size);
          std::vector<unsigned char> _output(chunk_size + block_size);

          while (!plain.eof())
          {
            // Read the plain's input stream and put a block of data in a
            // temporary buffer.
            plain.read(reinterpret_cast<char*>(_input.data()), _input.size());
            if (plain.bad())
              throw Exception(
                elle::sprintf("unable to read the plain's input stream: %s",
                              plain.rdstate()));

            int size_update(0);

            // Encrypt the input buffer.
            if (::EVP_EncryptUpdate(&context,
                                    _output.data(),
                                    &size_update,
                                    _input.data(),
                                    plain.gcount()) == 0)
            throw Exception(
              elle::sprintf("unable to apply the encryption function: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

            // Write the output buffer to the code stream.
            code.write(reinterpret_cast<const char *>(_output.data()),
                       size_update);
            if (!code.good())
              throw Exception(
                elle::sprintf("unable to write the encrypted data to the "
                              "code's output stream: %s",
                              code.rdstate()));
          }

          // Finalize the encryption process.
          int size_final(0);

          if (::EVP_EncryptFinal_ex(&context,
                                    _output.data(),
                                    &size_final) == 0)
            throw Exception(
              elle::sprintf("unable to finalize the encryption process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Write the final output buffer to the code's output stream.
          code.write(reinterpret_cast<const char *>(_output.data()),
                     size_final);
          if (!code.good())
            throw Exception(
              elle::sprintf("unable to write the encrypted data to the "
                            "code's output stream: %s",
                            code.rdstate()));

          // Clean up the cipher context.
          if (::EVP_CIPHER_CTX_cleanup(&context) <= 0)
            throw Exception(
              elle::sprintf("unable to clean the cipher context: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(context);
        }

        void
        decipher(std::istream& code,
                 std::ostream& plain,
                 elle::ConstWeakBuffer const& secret,
                 ::EVP_CIPHER const* function_cipher,
                 ::EVP_MD const* function_oneway)
        {
          ELLE_DEBUG_FUNCTION(function_cipher, function_oneway);
          ELLE_DUMP("secret: %s", secret);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          // Check whether the code was produced with a salt.
          elle::Character _magic[sizeof (magic)];

          code.read(reinterpret_cast<char*>(_magic), sizeof (magic) - 1);
          if (!code.good())
            throw Exception(
              elle::sprintf("unable to read the magic from the code's "
                            "input stream: %s",
                            code.rdstate()));

          if (::memcmp(_magic,
                       magic,
                       sizeof (magic) - 1) != 0)
            throw Exception("the code was produced without any or an invalid "
                            "salt");

          // Copy the salt for the sack of clarity.
          unsigned char _salt[PKCS5_SALT_LEN];

          code.read(reinterpret_cast<char*>(_salt), sizeof (_salt));
          if (!code.good())
            throw Exception(
              elle::sprintf("unable to read the salt from the code's "
                            "input stream: %s",
                            code.rdstate()));

          // Generate the key/IV tuple based on the salt.
          unsigned char key[EVP_MAX_KEY_LENGTH];
          unsigned char iv[EVP_MAX_IV_LENGTH];

          if (::EVP_BytesToKey(function_cipher,
                               function_oneway,
                               _salt,
                               secret.contents(),
                               secret.size(),
                               1,
                               key,
                               iv) > static_cast<int>(sizeof (key)))
            throw Exception("the generated key size is too large");

          // Initialize the cipher context.
          ::EVP_CIPHER_CTX context;

          ::EVP_CIPHER_CTX_init(&context);

          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_CLEANUP_CIPHER_CONTEXT(context);

          // Initialise the ciphering process.
          if (::EVP_DecryptInit_ex(&context,
                                   function_cipher,
                                   nullptr,
                                   key,
                                   iv) == 0)
            throw Exception(
              elle::sprintf("unable to initialize the decryption process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Retreive the cipher-specific block size.
          int block_size = ::EVP_CIPHER_CTX_block_size(&context);

          // Decipher the code's stream.
          std::vector<unsigned char> _input(chunk_size);
          std::vector<unsigned char> _output(chunk_size + block_size);

          while (!code.eof())
          {
            // Read the code's input stream and put a block of data in a
            // temporary buffer.
            code.read(reinterpret_cast<char*>(_input.data()), _input.size());
            if (code.bad())
              throw Exception(
                elle::sprintf("unable to read the code's input stream: %s",
                              code.rdstate()));

            // Decrypt the input buffer.
            int size_update(0);

            if (::EVP_DecryptUpdate(&context,
                                    _output.data(),
                                    &size_update,
                                    _input.data(),
                                    code.gcount()) == 0)
              throw Exception(
                elle::sprintf("unable to apply the decryption function: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));

            // Write the output buffer to the plain stream.
            plain.write(reinterpret_cast<const char *>(_output.data()),
                        size_update);
            if (!plain.good())
              throw Exception(
                elle::sprintf("unable to write the decrypted data to the "
                              "plain's output stream: %s",
                              plain.rdstate()));
          }

          // Finalise the deciphering process.
          int size_final(0);

          if (::EVP_DecryptFinal_ex(&context,
                                    _output.data(),
                                    &size_final) == 0)
            throw Exception(
              elle::sprintf("unable to finalize the decryption process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Write the final output buffer to the plain's output stream.
          plain.write(reinterpret_cast<const char *>(_output.data()),
                      size_final);
          if (!plain.good())
            throw Exception(
              elle::sprintf("unable to write the decrypted data to the "
                            "plain's output stream: %s",
                            plain.rdstate()));

          // Clean up the cipher context.
          if (::EVP_CIPHER_CTX_cleanup(&context) <= 0)
            throw Exception(
              elle::sprintf("unable to clean the cipher context: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(context);
        }
      }
    }
  }
}

//
// ---------- Hash ------------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace evp
    {
      elle::Buffer
      hash(std::istream& plain,
           ::EVP_MD const* function)
      {
        ELLE_DEBUG_FUNCTION(function);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Initialise the context.
        ::EVP_MD_CTX context;

        ::EVP_MD_CTX_init(&context);

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_CLEANUP_DIGEST_CONTEXT(context);

        // Initialise the digest.
        if (::EVP_DigestInit_ex(&context, function, nullptr) <= 0)
          throw Exception(
            elle::sprintf("unable to initialize the digest process: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        // Hash the plain's stream.
        std::vector<unsigned char> _input(chunk_size);

        while (!plain.eof())
        {
          // Read the plain's input stream and put a block of data in a
          // temporary buffer.
          plain.read(reinterpret_cast<char*>(_input.data()), _input.size());
          if (plain.bad())
            throw Exception(
              elle::sprintf("unable to read the plain's input stream: %s",
                            plain.rdstate()));

          // Update the digest context.
          if (::EVP_DigestUpdate(&context,
                                 _input.data(),
                                 plain.gcount()) <= 0)
            throw Exception(
              elle::sprintf("unable to apply the digest function: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));
        }

        // Allocate the output digest.
        elle::Buffer digest(EVP_MD_size(function));

        // Finalise the digest.
        unsigned int size(0);

        if (::EVP_DigestFinal_ex(&context,
                                 digest.mutable_contents(),
                                 &size) <= 0)
          throw Exception(
            elle::sprintf("unable to finalize the digest process: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        // Update the digest final size.
        digest.size(size);
        digest.shrink_to_fit();

        // Clean the context.
        if (::EVP_MD_CTX_cleanup(&context) <= 0)
          throw Exception(
            elle::sprintf("unable to clean the digest context: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(context);

        return (digest);
      }
    }
  }
}

//
// ---------- HMAC ------------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace evp
    {
      namespace hmac
      {
        elle::Buffer
        sign(std::istream& plain,
             ::EVP_PKEY* key,
             ::EVP_MD const* function)
        {
          ELLE_DEBUG_FUNCTION(key, function);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          // Initialise the context.
          ::EVP_MD_CTX context;

          ::EVP_MD_CTX_init(&context);

          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_CLEANUP_DIGEST_CONTEXT(context);

          if (::EVP_DigestInit_ex(&context, function, NULL) <= 0)
            throw Exception(
              elle::sprintf("unable to initialize the digest function: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          if (::EVP_DigestSignInit(&context, NULL, function, NULL, key) <= 0)
            throw Exception(
              elle::sprintf("unable to initialize the HMAC process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // HMAC-sign the plain's stream.
          std::vector<unsigned char> _input(chunk_size);

          while (!plain.eof())
          {
            // Read the plain's input stream and put a block of data in a
            // temporary buffer.
            plain.read(reinterpret_cast<char*>(_input.data()), _input.size());
            if (plain.bad())
              throw Exception(
                elle::sprintf("unable to read the plain's input stream: %s",
                              plain.rdstate()));

            // Update the HMAC.
            if (::EVP_DigestSignUpdate(&context,
                                       _input.data(),
                                       plain.gcount()) <= 0)
              throw Exception(
                elle::sprintf("unable to apply the HMAC function: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));
          }

          // Compute the output length.
          unsigned long size(0);

          if (::EVP_DigestSignFinal(&context,
                                    nullptr,
                                    &size) <= 0)
            throw Exception(
              elle::sprintf("unable to compute the final HMAC size: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Finalise the digest.
          elle::Buffer digest(size);

          if (::EVP_DigestSignFinal(&context,
                                    digest.mutable_contents(),
                                    &size) <= 0)
            throw Exception(
              elle::sprintf("unable to finalize the digest process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ELLE_ASSERT_EQ(digest.size(), size);

          // Update the digest final size.
          digest.size(size);
          digest.shrink_to_fit();

          // Clean the context.
          if (::EVP_MD_CTX_cleanup(&context) <= 0)
            throw Exception(
              elle::sprintf("unable to clean the digest context: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(context);

          return (digest);
        }

        elle::Boolean
        verify(elle::ConstWeakBuffer const& digest,
               std::istream& plain,
               ::EVP_PKEY* key,
               ::EVP_MD const* function)
        {
          ELLE_DEBUG_FUNCTION(key, function);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          // Initialise the context.
          ::EVP_MD_CTX context;

          ::EVP_MD_CTX_init(&context);

          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_CLEANUP_DIGEST_CONTEXT(context);

          if (::EVP_DigestInit_ex(&context, function, NULL) <= 0)
            throw Exception(
              elle::sprintf("unable to initialize the digest function: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          if (::EVP_DigestVerifyInit(&context, NULL, function, NULL, key) <= 0)
            throw Exception(
              elle::sprintf("unable to initialize the HMAC process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // HMAC-verify the plain's stream against the digest.
          std::vector<unsigned char> _input(chunk_size);

          while (!plain.eof())
          {
            // Read the plain's input stream and put a block of data in a
            // temporary buffer.
            plain.read(reinterpret_cast<char*>(_input.data()), _input.size());
            if (plain.bad())
              throw Exception(
                elle::sprintf("unable to read the plain's input stream: %s",
                              plain.rdstate()));

            // Update the HMAC.
            if (::EVP_DigestVerifyUpdate(&context,
                                         _input.data(),
                                         plain.gcount()) <= 0)
              throw Exception(
                elle::sprintf("unable to apply the HMAC function: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));
          }

          // Verify the context's data against the given digest.
          if (::EVP_DigestVerifyFinal(&context,
                                      digest.contents(),
                                      digest.size()) == 0)
            return (false);

          // Clean the context.
          if (::EVP_MD_CTX_cleanup(&context) <= 0)
            throw Exception(
              elle::sprintf("unable to clean the digest context: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(context);

          return (true);
        }
      }
    }
  }
}
