#include <cryptography/evp.hh>
#include <cryptography/SecretKey.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/Plain.hh>
#include <cryptography/Signature.hh>
#include <cryptography/oneway.hh>

#include <elle/Buffer.hh>
#include <elle/log.hh>

#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/err.h>

ELLE_LOG_COMPONENT("infinit.cryptography.evp");

namespace infinit
{
  namespace cryptography
  {
    namespace evp
    {
      /*----------.
      | Constants |
      `----------*/

      /// The length the the one-time secret key generated whenever
      /// the encryption is requested.
      ///
      /// Note that the length is in bits.
      static elle::Natural32 const secret_length = 256;

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
        SecretKey secret = SecretKey::generate(secret_length);

        // 2) Cipher the plain text with the secret key.
        Cipher data = secret.encrypt(plain);

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
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

        // Prepare the key code for receiving the encrypted secret.
        Code key{size};

        // Encrypt the secret key's archive.
        if (function(context,
                     reinterpret_cast<unsigned char*>(
                       key.buffer().mutable_contents()),
                     &size,
                     reinterpret_cast<const unsigned char*>(
                       buffer.contents()),
                     buffer.size()) <= 0)
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

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
        /* XXX
        Code key{code.buffer().reader()};
        Cipher data{code.buffer().reader()};

        // XXX
        elle::printf("KEY: %s\n", key);
        elle::printf("DATA: %s\n", data);
        */

        Code key;
        Cipher data;

        code.buffer().reader() >> key >> data;

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
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

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
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

        // Set the final buffer size.
        buffer.size(size);

        // Finally extract the secret key since decrypted.
        SecretKey secret;

        buffer.reader() >> secret;

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
        // Compute the plain's digest.
        Digest digest = oneway::hash(plain, oneway_algorithm);

        // Retrieve information on the size of the output signature.
        ::size_t size;

        ELLE_ASSERT(context != nullptr);
        ELLE_ASSERT(digest.buffer().contents() != nullptr);

        if (function(
              context,
              nullptr,
              &size,
              reinterpret_cast<const unsigned char*>(
                digest.buffer().contents()),
              digest.buffer().size()) <= 0)
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

        // Prepare the signature.
        Signature signature{size};

        // Perform the signing process.
        if (function(
              context,
              reinterpret_cast<unsigned char*>(
                signature.buffer().mutable_contents()),
              &size,
              reinterpret_cast<const unsigned char*>(
                digest.buffer().contents()),
              digest.buffer().size()) <= 0)
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

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
          function(
            context,
            reinterpret_cast<const unsigned char*>(
              signature.buffer().contents()),
            signature.buffer().size(),
            reinterpret_cast<const unsigned char*>(digest.buffer().contents()),
            digest.buffer().size());

        switch (result)
          {
          case 1:
            return (true);
          case 0:
            return (false);
          default:
            throw elle::Exception("%s",
                                  ::ERR_error_string(ERR_get_error(), nullptr));
          }

        elle::unreachable();
      }
    }
  }
}
