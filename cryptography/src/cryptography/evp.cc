#include <cryptography/evp.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/SecretKey.hh>
#include <cryptography/cipher.hh>
#include <cryptography/Plain.hh>
#include <cryptography/Signature.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/finally.hh>
#include <cryptography/Exception.hh>

#include <elle/Buffer.hh>
#include <elle/log.hh>

#include <openssl/crypto.h>
#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/hmac.h>
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
        /*-----------------.
        | Static Functions |
        `-----------------*/

        // Return the difference between a secret key in its serialized form
        // and the originally requested length for this key.
        //
        // This overhead therefore represents the additional bits implied by
        // the serialization mechanism.
        static
        elle::Natural32
        _overhead()
        {
          // The static password used to construct this dummy secret key
          // whose only purpose is to calculate the serialization overhead.
          elle::String password =
            "An equation means nothing to me unless it expresses "
            "a thought of God. Srinivasa Ramanujan";
          elle::Natural32 length_original = password.length() * 8;

          // The cipher is not important as it does not impact the
          // serialization's overhead.
          Cipher cipher = Cipher::aes256;
          Oneway oneway = Oneway::sha256;
          SecretKey secret(cipher, password, oneway);

          elle::Buffer buffer;
          buffer.writer() << secret;

          elle::Natural32 length_final = buffer.size() * 8;

          ELLE_ASSERT_GTE(length_final, length_original);

          // Return the difference between the requested secret length and
          // the actual one.
          return (length_final - length_original);
        }

        elle::Buffer
        apply(elle::WeakBuffer const& input,
              ::EVP_PKEY_CTX* context,
              int (*function)(EVP_PKEY_CTX*,
                              unsigned char*,
                              size_t*,
                              const unsigned char*,
                              size_t))
        {
          // Make sure the cryptographic system is set up.
          cryptography::require();

          ELLE_ASSERT_GTE(static_cast<elle::Natural32>(
                            ::EVP_PKEY_bits(::EVP_PKEY_CTX_get0_pkey(context))),
                          input.size() * 8);

          // Compute the size of the encrypted buffer.
          ::size_t size;

          ELLE_ASSERT_NEQ(context, nullptr);
          ELLE_ASSERT_NEQ(input.contents(), nullptr);

          if (function(context,
                       nullptr,
                       &size,
                       input.contents(),
                       input.size()) <= 0)
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
                       input.contents(),
                       input.size()) <= 0)
            throw Exception(elle::sprintf("unable to apply the cryptographic "
                                          "function: %s",
                                          ::ERR_error_string(ERR_get_error(),
                                                             nullptr)));

          // Set the final output buffer size.
          output.size(size);
          output.shrink_to_fit();

          return (output);
        }

        Code
        encrypt(Plain const& plain,
                ::EVP_PKEY_CTX* context,
                int (*function)(EVP_PKEY_CTX*,
                                unsigned char*,
                                size_t*,
                                const unsigned char*,
                                size_t),
                Cipher const cipher,
                Oneway const oneway,
                elle::Natural32 const padding_size)
        {
          ELLE_TRACE_SCOPE("encrypt(%s, %s, %s, %s)",
                           context, function, cipher, padding_size);
          ELLE_DUMP("plain: %s", plain);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          // The overhead represents the difference between a bare secret
          // key length and the same key in its serialized form. Note that
          // the overhead is expressed in bits.
          //
          // Without computing this difference, one may think the following
          // is going to encrypt the temporary secret key with the given
          // context, ending up with an output code key looking as below:
          //
          //   [symmetrically encrypted secret][padding]
          //
          // Unfortunately, before asymmetrically encrypting the secret key,
          // one must serialize it. Since the serialization mechanism adds
          // a few bytes itself, the final layout ressembles more the following:
          //
          //   [symmetrically encrypted [serialized overhead/secret]][padding]
          //
          // The following calculates the overhead generated by serialization to
          // compute the right length for the secret key.
          //
          // Note that this overhead is only calculated once as it does not
          // depend on the nature of the secret key that will be generated.
          static elle::Natural32 overhead = _overhead();

          // 1) Compute the size of the secret key to generate by taking
          //    into account the padding size, if any.
           ELLE_ASSERT_GTE(static_cast<elle::Natural32>(
                            ::EVP_PKEY_bits(::EVP_PKEY_CTX_get0_pkey(context))),
                          (padding_size + overhead));

          // The maximum length, in bits, of the generated symmetric key.
          elle::Natural32 const ceiling = 512;
          elle::Natural32 const length =
            (::EVP_PKEY_bits(::EVP_PKEY_CTX_get0_pkey(context)) -
             (padding_size + overhead)) < ceiling ?
            ::EVP_PKEY_bits(::EVP_PKEY_CTX_get0_pkey(context)) -
            (padding_size + overhead) : ceiling;

          // 2) Generate a temporary secret key.
          SecretKey secret = secretkey::generate(cipher, length, oneway);

          // 3) Cipher the plain text with the secret key.
          Code data = secret.encrypt(plain);

          // 4) Asymmetrically encrypt the secret with the given function
          //    and encryption context.

          // Serialize the secret.
          elle::Buffer buffer;

          buffer.writer() << secret;

          ELLE_ASSERT_GT(buffer.size(), 0u);

          // Encrypt the secret key's archive.
          Code key(apply(buffer,
                         context,
                         function));

          // 5) Serialize the asymmetrically encrypted key and the symmetrically
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
          ELLE_TRACE_SCOPE("decrypt(%s, %s)", context, function);
          ELLE_DUMP("code: %s", code);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          // 1) Extract the key and ciphered data from the code which
          //    is supposed to be an archive.
          ELLE_DEBUG("extract symmetric key and message");
          auto extractor = code.buffer().reader();
          Code key(extractor);
          Code data(extractor);

          // 2) Decrypt the key so as to reveal the symmetric secret key.

          // Decrypt the key.
          ELLE_DEBUG("decrypt symmetric key");
          elle::Buffer buffer = apply(key.buffer(),
                                      context,
                                      function);
          // Finally extract the secret key since decrypted.
          SecretKey secret(buffer.reader());

          // 3) Decrypt the data with the secret key.
          ELLE_DEBUG("decrypt message");
          Clear clear = secret.decrypt(data);

          return (clear);
        }

        Signature
        sign(Digest const& digest,
             ::EVP_PKEY_CTX* context,
             int (*function)(EVP_PKEY_CTX*,
                             unsigned char*,
                             size_t*,
                             const unsigned char*,
                             size_t))
        {
          ELLE_TRACE_SCOPE("sign(%s, %s)", context, function);
          ELLE_DUMP("digest: %s", digest);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          Signature signature(apply(digest.buffer(),
                                    context,
                                    function));

          return (signature);
        }

        elle::Boolean
        verify(Signature const& signature,
               Digest const& digest,
               ::EVP_PKEY_CTX* context,
               int (*function)(EVP_PKEY_CTX*,
                               const unsigned char*,
                               size_t,
                               const unsigned char*,
                               size_t))
        {
          ELLE_TRACE_SCOPE("verify(%s, %s)", context, function);
          ELLE_DUMP("signature: %s", signature);
          ELLE_DUMP("digest: %s", digest);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          ELLE_ASSERT_NEQ(context, nullptr);
          ELLE_ASSERT_NEQ(signature.buffer().contents(), nullptr);
          ELLE_ASSERT_NEQ(digest.buffer().contents(), nullptr);

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
          ELLE_TRACE_SCOPE("encrypt(%s, %s)", function_cipher, function_oneway);
          ELLE_DUMP("plain: %s", plain);
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

          ELLE_ASSERT_NEQ(plain.buffer().contents(), nullptr);

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
          code.buffer().shrink_to_fit();

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
          ELLE_TRACE_SCOPE("decrypt(%s, %s)", function_cipher, function_oneway);
          ELLE_DUMP("code: %s", code);
          ELLE_DUMP("secret: %s", secret);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          // Check whether the code was produced with a salt.
          ELLE_ASSERT_NEQ(code.buffer().contents(), nullptr);

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
          ELLE_ASSERT_NEQ(secret.contents(), nullptr);

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
          clear.buffer().shrink_to_fit();

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
          ELLE_TRACE_SCOPE("hash(%s)", function);
          ELLE_DUMP("plain: %s", plain);

          // Make sure the cryptographic system is set up.
          cryptography::require();

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

          // Normalize behavior between no data and data of size 0.
          // by always forcing it into the second
          elle::Byte const* data = reinterpret_cast<const elle::Byte*>("");
          elle::Buffer::Size length = 0;
          if (plain.buffer().contents() != nullptr)
          {
            data = plain.buffer().contents();
            length = plain.buffer().size();
          }

          // Update the digest with the given plain's data.
          if (::EVP_DigestUpdate(&context,
                                 data,
                                 length) <= 0)
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
          digest.buffer().shrink_to_fit();

          // Clean the context.
          if (::EVP_MD_CTX_cleanup(&context) <= 0)
            throw Exception(
              elle::sprintf("unable to clean the digest context: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(context);

          return (digest);
        }

        Digest
        hmac(Plain const& plain,
             Digest const& key,
             ::EVP_MD const* function)
        {
          ELLE_TRACE_SCOPE("hmac(%s)", function);
          ELLE_DUMP("plain: %s", plain);
          ELLE_DUMP("key: %s", key);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          Digest digest(EVP_MD_size(function));

          // Initialise the context.
          ::HMAC_CTX context;

          ::HMAC_CTX_init(&context);

          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_CLEANUP_HMAC_DIGEST_CONTEXT(
            context);

          // Initialise the digest.
          if (::HMAC_Init_ex(&context, key.buffer().contents(),
                             key.buffer().size(), function, nullptr) <= 0)
            throw Exception(
              elle::sprintf("unable to initialize the digest process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ELLE_ASSERT_NEQ(plain.buffer().contents(), nullptr);

          // Update the digest with the given plain's data.
          if (::HMAC_Update(&context,
                            plain.buffer().contents(),
                            plain.buffer().size()) <= 0)
            throw Exception(
              elle::sprintf("unable to apply the digest process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Finalise the digest.
          unsigned int size;

          if (::HMAC_Final(&context,
                           digest.buffer().mutable_contents(),
                           &size) <=0)
            throw Exception(
              elle::sprintf("unable to finalize the digest process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Update the digest final size.
          digest.buffer().size(size);
          digest.buffer().shrink_to_fit();

          // Clean the context.
          ::HMAC_CTX_cleanup(&context);

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(context);

          return (digest);
        }
      }
    }
  }
}
