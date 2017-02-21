#include <openssl/crypto.h>
#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>

#include <elle/Buffer.hh>
#include <elle/log.hh>
#include <elle/serialization/binary.hh>

#include <elle/cryptography/Cipher.hh>
#include <elle/cryptography/Oneway.hh>
#include <elle/cryptography/Error.hh>
#include <elle/cryptography/SecretKey.hh>
#include <elle/cryptography/cryptography.hh>
#include <elle/cryptography/envelope.hh>
#include <elle/cryptography/finally.hh>
#include <elle/cryptography/raw.hh>
#include <elle/cryptography/constants.hh>

namespace elle
{
  namespace cryptography
  {
    namespace envelope
    {
      /*----------.
      | Functions |
      `----------*/

      void
      seal(::EVP_PKEY* key,
           ::EVP_CIPHER const* cipher,
           std::istream& plain,
           std::ostream& code)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        // The following variables initialization are more complicated than
        // necessary but have been made for the reader to understand the way
        // the SealInit() function takes arguments, arrays and not single
        // values.

        // Create an array with a single public key since only one recipient
        // will be allowed to decrypt the data.
        ::EVP_PKEY* keys[1];
        keys[0] = key;

        // Create an array with a single secret key.
        unsigned char* _secret =
          reinterpret_cast<unsigned char*>(
            ::OPENSSL_malloc(::EVP_PKEY_size(keys[0])));
        ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_OPENSSL(_secret);

        unsigned char* secrets[1];
        secrets[0] = _secret;
        int lengths[1];

        // Prepare the memory area to receive the IV.
        unsigned char* iv =
          reinterpret_cast<unsigned char*>(
            ::OPENSSL_malloc(EVP_MAX_IV_LENGTH));
        ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_OPENSSL(iv);

        // Initialize the cipher context.
        ::EVP_CIPHER_CTX context;

        ::EVP_CIPHER_CTX_init(&context);

        ELLE_CRYPTOGRAPHY_FINALLY_ACTION_CLEANUP_CIPHER_CONTEXT(context);

        // Initialize the envelope seal operation. This operation generates
        // a secret key for the provided cipher, and then encrypts that key a
        // number of times (one for each public key provided in the 'keys'
        // array, once in our case). This operation also generates an IV and
        // places it in 'iv'.
        if (::EVP_SealInit(&context,
                           cipher,
                           secrets,
                           lengths,
                           iv,
                           keys,
                           1) <= 0)
          throw Error(
            elle::sprintf("unable to initialize the seal process: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        ELLE_ASSERT_EQ(::EVP_PKEY_size(key), lengths[0]);

        // At this point, before writing the encrypted data in the output
        // stream, the parameters (secret and IV) need to be written for
        // the recipient to retrieve them.
        //
        // We could for instance serialize the header (secret and IV) through
        // a serialization layer on top of the output stream and then use the
        // bare stream to output the encrypted data. Because the serialization
        // layer may buffer, it is dangerous. Likewise, the serialization format
        // may change in the future.
        //
        // For those reasons, the following simply outputs the secret and IV
        // whose size is supposed to be known in advanced according to the
        // cipher used.
        {
          // Write the secret.
          code.write(reinterpret_cast<const char *>(_secret),
                     ::EVP_PKEY_size(key));
          if (!code.good())
            throw Error(
              elle::sprintf("unable to write the secret to the "
                            "code's output stream: %s",
                            code.rdstate()));

          // Write the IV.
          code.write(reinterpret_cast<const char *>(iv),
                     EVP_MAX_IV_LENGTH);
          if (!code.good())
            throw Error(
              elle::sprintf("unable to write the IV to the "
                            "code's output stream: %s",
                            code.rdstate()));
        }

        // Compute the block size according to the
        int block_size = ::EVP_CIPHER_CTX_block_size(&context);

        // Encrypt the plain's stream.
        std::vector<unsigned char> _input(constants::stream_block_size);
        std::vector<unsigned char> _output(constants::stream_block_size +
                                           block_size);

        while (!plain.eof())
        {
          // Read the plain's input stream and put a block of data in a
          // temporary buffer.
          plain.read(reinterpret_cast<char*>(_input.data()), _input.size());
          if (plain.bad())
            throw Error(
              elle::sprintf("unable to read the plain's input stream: %s",
                            plain.rdstate()));

          int size_update(0);

          // Encrypt the input buffer.
          if (::EVP_SealUpdate(&context,
                               _output.data(),
                               &size_update,
                               _input.data(),
                               plain.gcount()) <= 0)
            throw Error(
              elle::sprintf("unable to apply the encryption function: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Write the output buffer to the code stream.
          code.write(reinterpret_cast<const char *>(_output.data()),
                     size_update);
          if (!code.good())
            throw Error(
              elle::sprintf("unable to write the encrypted data to the "
                            "code's output stream: %s",
                            code.rdstate()));
        }

        // Finalize the encryption process.
        int size_final(0);

        if (::EVP_SealFinal(&context,
                            _output.data(),
                            &size_final) <= 0)
          throw Error(
            elle::sprintf("unable to finalize the seal process: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        // Write the final output buffer to the code's output stream.
        code.write(reinterpret_cast<const char *>(_output.data()),
                   size_final);
        if (!code.good())
          throw Error(
            elle::sprintf("unable to write the encrypted data to the "
                          "code's output stream: %s",
                          code.rdstate()));

        // Clean up the cipher context.
        if (::EVP_CIPHER_CTX_cleanup(&context) <= 0)
          throw Error(
            elle::sprintf("unable to clean the cipher context: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
        ELLE_CRYPTOGRAPHY_FINALLY_ABORT(context);

        // Release the memory associated with the secret and iv.
        ::OPENSSL_free(iv);
        ELLE_CRYPTOGRAPHY_FINALLY_ABORT(iv);

        ::OPENSSL_free(_secret);
        ELLE_CRYPTOGRAPHY_FINALLY_ABORT(_secret);
      }

      void
      open(::EVP_PKEY* key,
           ::EVP_CIPHER const* cipher,
           std::istream& code,
           std::ostream& plain)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Start by extracting the secret and IV from the input
        // stream.
        unsigned char* secret =
          reinterpret_cast<unsigned char*>(
            ::OPENSSL_malloc(::EVP_PKEY_size(key)));
        ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_OPENSSL(secret);

        unsigned char* iv =
          reinterpret_cast<unsigned char*>(
            ::OPENSSL_malloc(EVP_MAX_IV_LENGTH));
        ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_OPENSSL(iv);

        {
          // Read the secret.
          code.read(reinterpret_cast<char*>(secret), ::EVP_PKEY_size(key));
          if (!code.good())
            throw Error(
              elle::sprintf("unable to read the secret from the code's "
                            "input stream: %s",
                            code.rdstate()));

          // Read the IV.
          code.read(reinterpret_cast<char*>(iv), EVP_MAX_IV_LENGTH);
          if (!code.good())
            throw Error(
              elle::sprintf("unable to read the IV from the code's "
                            "input stream: %s",
                            code.rdstate()));
        }

        // Initialize the cipher context.
        ::EVP_CIPHER_CTX context;

        ::EVP_CIPHER_CTX_init(&context);

        ELLE_CRYPTOGRAPHY_FINALLY_ACTION_CLEANUP_CIPHER_CONTEXT(context);

        // Initialize the envelope open operation.
        if (::EVP_OpenInit(&context,
                           cipher,
                           secret,
                           ::EVP_PKEY_size(key),
                           iv,
                           key) <= 0)
          throw Error(
            elle::sprintf("unable to initialize the open process: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        // Compute the block size according to the
        int block_size = ::EVP_CIPHER_CTX_block_size(&context);

        // Decrypt the plain's stream.
        std::vector<unsigned char> _input(constants::stream_block_size);
        std::vector<unsigned char> _output(constants::stream_block_size +
                                           block_size);

        while (!code.eof())
        {
          // Read the code's input stream and put a block of data in a
          // temporary buffer.
          code.read(reinterpret_cast<char*>(_input.data()), _input.size());
          if (code.bad())
            throw Error(
              elle::sprintf("unable to read the code's input stream: %s",
                            code.rdstate()));

          int size_update(0);

          // Decrypt the input buffer.
          if (::EVP_OpenUpdate(&context,
                               _output.data(),
                               &size_update,
                               _input.data(),
                               code.gcount()) <= 0)
            throw Error(
              elle::sprintf("unable to apply the decryption function: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Write the output buffer to the plain stream.
          plain.write(reinterpret_cast<const char *>(_output.data()),
                      size_update);
          if (!plain.good())
            throw Error(
              elle::sprintf("unable to write the decrypted data to the "
                            "plain's output stream: %s",
                            plain.rdstate()));
        }

        // Finalize the decryption process.
        int size_final(0);

        if (::EVP_OpenFinal(&context,
                            _output.data(),
                            &size_final) <= 0)
          throw Error(
            elle::sprintf("unable to finalize the open process: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        // Write the final output buffer to the plain's output stream.
        plain.write(reinterpret_cast<const char *>(_output.data()),
                   size_final);
        if (!plain.good())
          throw Error(
            elle::sprintf("unable to write the decrypted data to the "
                          "plain's output stream: %s",
                          plain.rdstate()));

        // Clean up the cipher context.
        if (::EVP_CIPHER_CTX_cleanup(&context) <= 0)
          throw Error(
            elle::sprintf("unable to clean the cipher context: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
        ELLE_CRYPTOGRAPHY_FINALLY_ABORT(context);

        // Release the memory associated with the secret and iv.
        OPENSSL_free(iv);
        ELLE_CRYPTOGRAPHY_FINALLY_ABORT(iv);

        OPENSSL_free(secret);
        ELLE_CRYPTOGRAPHY_FINALLY_ABORT(secret);
      }
    }
  }
}
