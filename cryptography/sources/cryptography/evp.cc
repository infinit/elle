#include <cryptography/evp.hh>
#include <cryptography/SecretKey.hh>
#include <cryptography/cipher.hh>
#include <cryptography/Plain.hh>
#include <cryptography/Signature.hh>
#include <cryptography/oneway.hh>
#include <cryptography/finally.hh>
#include <cryptography/Exception.hh>

#include <elle/Buffer.hh>
#include <elle/log.hh>

#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/rand.h>

ELLE_LOG_COMPONENT("infinit.cryptography.evp");

//
// ---------- Asymmetric ------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace evp
    {
      namespace asymmetric
      {
        /*----------.
        | Constants |
        `----------*/

        /// The length the the one-time secret key generated whenever
        /// the encryption is requested.
        ///
        /// Note that the length is in bits.
        static elle::Natural32 const secret_length = 256;
        /// The cipher algorithm used for encrypting the data.
        static cipher::Algorithm const cipher_algorithm =
          cipher::Algorithm::aes256;
        /// The algorithm used for hashing the content to sign/verify.
        static oneway::Algorithm const oneway_algorithm =
          oneway::Algorithm::sha256;

        /*----------.
        | Functions |
        `----------*/

        Code
        encrypt(Plain const& plain,
                ::EVP_PKEY_CTX* context,
                int (*function)(EVP_PKEY_CTX*,
                                unsigned char*,
                                size_t*,
                                const unsigned char*,
                                size_t))
        {
          ELLE_TRACE_FUNCTION(plain, context, function);

          // 1) Generate a temporary secret key.
          SecretKey secret = SecretKey::generate(cipher_algorithm,
                                                 secret_length);

          // 2) Cipher the plain text with the secret key.
          Code data = secret.encrypt(plain);

          // 3) Asymmetrically encrypt the secret with the given function
          //    and encryption context.

          // Serialize the secret.
          elle::Buffer buffer;

          buffer.writer() << secret;

          ELLE_ASSERT(buffer.size() > 0);

          // Compute the size of the archived symmetric key.
          ::size_t size;

          ELLE_ASSERT(context != nullptr);
          ELLE_ASSERT(buffer.contents() != nullptr);

          if (function(context,
                       nullptr,
                       &size,
                       buffer.contents(),
                       buffer.size()) <= 0)
            throw Exception(elle::sprintf("unable to pre-compute the size of "
                                          "the encrypted output: %s",
                                          ::ERR_error_string(ERR_get_error(),
                                                             nullptr)));

          // Prepare the key code for receiving the encrypted secret.
          Code key(size);

          // Encrypt the secret key's archive.
          if (function(context,
                       reinterpret_cast<unsigned char*>(
                         key.buffer().mutable_contents()),
                       &size,
                       reinterpret_cast<const unsigned char*>(
                         buffer.contents()),
                       buffer.size()) <= 0)
            throw Exception(elle::sprintf("unable to apply the encryption "
                                          "process: %s",
                                          ::ERR_error_string(ERR_get_error(),
                                                             nullptr)));

          // Set the final key size.
          key.buffer().size(size);

          // 4) Serialize the asymmetrically encrypted key and the symmetrically
          //    encrypted data.
          Code code;

          code.buffer().writer() << key << data;

          return (code);
        }

        Clear
        decrypt(Code const& code,
                ::EVP_PKEY_CTX* context,
                int (*function)(EVP_PKEY_CTX*,
                                unsigned char*,
                                size_t*,
                                const unsigned char*,
                                size_t))
        {
          ELLE_TRACE_FUNCTION(code, context, function);

          // 1) Extract the key and ciphered data from the code which
          //    is supposed to be an archive.
          auto extractor = code.buffer().reader();

          Code key(extractor);
          Code data(extractor);

          // 2) Decrypt the key so as to reveal the symmetric secret key.

          // Compute the size of the decrypted portion to come.
          ::size_t size;

          ELLE_ASSERT(context != nullptr);
          ELLE_ASSERT(key.buffer().contents() != nullptr);

          if (function(context,
                       nullptr,
                       &size,
                       reinterpret_cast<const unsigned char*>(
                         key.buffer().contents()),
                       key.buffer().size()) <= 0)
            throw Exception(
              elle::sprintf("unable to pre-compute the size of the decrypted "
                            "output: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Prepare the buffer for receiving the decrypted key's archive.
          elle::Buffer buffer(size);

          // Perform the decrypt operation.
          if (function(context,
                       reinterpret_cast<unsigned char*>(
                         buffer.mutable_contents()),
                       &size,
                       reinterpret_cast<const unsigned char*>(
                         key.buffer().contents()),
                       key.buffer().size()) <= 0)
            throw Exception(
              elle::sprintf("unable to apply the decryption process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Set the final buffer size.
          buffer.size(size);

          // Finally extract the secret key since decrypted.
          SecretKey secret(buffer.reader());

          // 3) Decrypt the data with the secret key.
          Clear clear = secret.decrypt(data);

          return (clear);
        }

        Signature
        sign(Plain const& plain,
             ::EVP_PKEY_CTX* context,
             int (*function)(EVP_PKEY_CTX*,
                             unsigned char*,
                             size_t*,
                             const unsigned char*,
                             size_t))
        {
          ELLE_TRACE_FUNCTION(plain, context, function);

          // Compute the plain's digest.
          Digest digest = oneway::hash(plain, oneway_algorithm);

          // Retrieve information on the size of the output signature.
          ::size_t size;

          ELLE_ASSERT(context != nullptr);
          ELLE_ASSERT(digest.buffer().contents() != nullptr);

          if (function(context,
                       nullptr,
                       &size,
                       reinterpret_cast<const unsigned char*>(
                         digest.buffer().contents()),
                       digest.buffer().size()) <= 0)
            throw Exception(
              elle::sprintf("unable to pre-compute the size of "
                            "the signature output: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Prepare the signature.
          Signature signature(size);

          // Perform the signing process.
          if (function(context,
                       reinterpret_cast<unsigned char*>(
                         signature.buffer().mutable_contents()),
                       &size,
                       reinterpret_cast<const unsigned char*>(
                         digest.buffer().contents()),
                       digest.buffer().size()) <= 0)
            throw Exception(
              elle::sprintf("unable to apply the signature process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Set the final signature size.
          signature.buffer().size(size);

          return (signature);
        }

        elle::Boolean
        verify(Signature const& signature,
               Plain const& plain,
               ::EVP_PKEY_CTX* context,
               int (*function)(EVP_PKEY_CTX*,
                               const unsigned char*,
                               size_t,
                               const unsigned char*,
                               size_t))
        {
          ELLE_TRACE_FUNCTION(signature, plain, context, function);

          // Compute the plain's digest.
          Digest digest = oneway::hash(plain, oneway_algorithm);

          ELLE_ASSERT(context != nullptr);
          ELLE_ASSERT(signature.buffer().contents() != nullptr);
          ELLE_ASSERT(digest.buffer().contents() != nullptr);

          // Verify the signature.
          int result =
            function(context,
                     reinterpret_cast<const unsigned char*>(
                       signature.buffer().contents()),
                     signature.buffer().size(),
                     reinterpret_cast<const unsigned char*>(
                       digest.buffer().contents()),
                     digest.buffer().size());

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

        Code
        encrypt(Plain const& plain,
                elle::Buffer const& secret,
                ::EVP_CIPHER const* function_cipher,
                ::EVP_MD const* function_oneway)
        {
          ELLE_TRACE_FUNCTION(plain, secret, function_cipher, function_oneway);

          // Generate a salt.
          unsigned char salt[PKCS5_SALT_LEN];

          ::RAND_pseudo_bytes(salt, sizeof (salt));

          // Check that the secret key's buffer has a non-null address.
          //
          // Otherwise, EVP_BytesToKey() is non-deterministic :(
          ELLE_ASSERT(secret.contents() != nullptr);

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
                               iv) > static_cast<int>(sizeof(key)))
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

          // Retreive the cipher-specific block size.
          int block_size = ::EVP_CIPHER_CTX_block_size(&context);

          // Allocate the code.
          Code code(sizeof (magic) -
                    1 +
                    sizeof (salt) +
                    plain.buffer().size() +
                    block_size);

          // Embed the magic directly into the code.
          ::memcpy(code.buffer().mutable_contents(),
                   magic,
                   sizeof (magic) - 1);

          // Copy the salt directly into the code.
          ::memcpy(code.buffer().mutable_contents() +
                   sizeof (magic) - 1,
                   salt,
                   sizeof (salt));

          // Initialise the code's size.
          int size_header(sizeof (magic) - 1 + sizeof (salt));

          // Cipher the plain text.
          int size_update(0);

          ELLE_ASSERT(plain.buffer().contents() != nullptr);

          if (::EVP_EncryptUpdate(&context,
                                  code.buffer().mutable_contents() +
                                  size_header,
                                  &size_update,
                                  plain.buffer().contents(),
                                  plain.buffer().size()) == 0)
            throw Exception(
              elle::sprintf("unable to apply the encryption process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Finialise the ciphering process.
          int size_finalize(0);

          if (::EVP_EncryptFinal_ex(&context,
                                    code.buffer().mutable_contents() +
                                    size_header + size_update,
                                    &size_finalize) == 0)
            throw Exception(
              elle::sprintf("unable to finalize the encryption process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Update the code size with the actual size of the generated data.
          code.buffer().size(size_header + size_update + size_finalize);

          // Clean up the cipher context.
          ::EVP_CIPHER_CTX_cleanup(&context);

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(context);

          return (code);
        }

        Clear
        decrypt(Code const& code,
                elle::Buffer const& secret,
                ::EVP_CIPHER const* function_cipher,
                ::EVP_MD const* function_oneway)
        {
          ELLE_TRACE_FUNCTION(code, secret, function_cipher, function_oneway);

          // Check whether the code was produced with a salt.
          ELLE_ASSERT(code.buffer().contents() != nullptr);

          if (::memcmp(magic,
                       code.buffer().contents(),
                       sizeof (magic) - 1) != 0)
            throw Exception("the code was produced without any salt");

          // Copy the salt for the sack of clarity.
          unsigned char salt[PKCS5_SALT_LEN];

          ::memcpy(salt,
                   code.buffer().contents() + sizeof (magic) - 1,
                   sizeof (salt));

          // Check that the secret key's buffer has a non-null address.
          //
          // Otherwise, EVP_BytesToKey() is non-deterministic :(
          ELLE_ASSERT(secret.contents() != nullptr);

          // Generate the key/IV tuple based on the salt.
          unsigned char key[EVP_MAX_KEY_LENGTH];
          unsigned char iv[EVP_MAX_IV_LENGTH];

          if (::EVP_BytesToKey(function_cipher,
                               function_oneway,
                               salt,
                               secret.contents(),
                               secret.size(),
                               1,
                               key,
                               iv) > static_cast<int>(sizeof(key)))
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

          // Allocate the clear;
          Clear clear(code.buffer().size() -
                      (sizeof (magic) - 1 + sizeof (salt)) +
                      block_size);

          // Decipher the code.
          int size_update(0);

          if (::EVP_DecryptUpdate(&context,
                                  clear.buffer().mutable_contents(),
                                  &size_update,
                                  code.buffer().contents() +
                                  sizeof (magic) - 1 + sizeof (salt),
                                  code.buffer().size() -
                                  (sizeof (magic) - 1 + sizeof (salt))) == 0)
            throw Exception(
              elle::sprintf("unable to apply the decryption process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Finalise the ciphering process.
          int size_final(0);

          if (::EVP_DecryptFinal_ex(&context,
                                    clear.buffer().mutable_contents() +
                                    size_update,
                                    &size_final) == 0)
            throw Exception(
              elle::sprintf("unable to finalize the decryption process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Update the clear size with the actual size of the data decrypted.
          clear.buffer().size(size_update + size_final);

          // Clean up the cipher context.
          ::EVP_CIPHER_CTX_cleanup(&context);

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(context);

          return (clear);
        }
      }
    }
  }
}

//
// ---------- Digest ----------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace evp
    {
      namespace digest
      {
        Digest
        hash(Plain const& plain,
             ::EVP_MD const* function)
        {
          ELLE_TRACE_FUNCTION(plain, function);

          Digest digest(EVP_MD_size(function));

          // Initialise the context.
          ::EVP_MD_CTX context;

          ::EVP_MD_CTX_init(&context);

          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_CLEANUP_DIGEST_CONTEXT(context);

          // Initialise the digest.
          if (::EVP_DigestInit_ex(&context, function, nullptr) <= 0)
            throw Exception(
              elle::sprintf("unable to initialize the digest process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ELLE_ASSERT(plain.buffer().contents() != nullptr);

          // Update the digest with the given plain's data.
          if (::EVP_DigestUpdate(&context,
                                 plain.buffer().contents(),
                                 plain.buffer().size()) <= 0)
            throw Exception(
              elle::sprintf("unable to apply the digest process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Finalise the digest.
          unsigned int size;

          if (::EVP_DigestFinal_ex(&context,
                                   digest.buffer().mutable_contents(),
                                   &size) <=0)
            throw Exception(
              elle::sprintf("unable to finalize the digest process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Update the digest final size.
          digest.buffer().size(size);

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
}
