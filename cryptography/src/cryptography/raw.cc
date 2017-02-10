#include <cryptography/raw.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/SecretKey.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/finally.hh>
#include <cryptography/Error.hh>
#include <cryptography/types.hh>
#include <cryptography/context.hh>
#include <cryptography/constants.hh>

#include <elle/Buffer.hh>
#include <elle/log.hh>
#include <elle/serialization/binary.hh>

#include <openssl/crypto.h>
#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>

#include <thread>

#if defined(ELLE_CRYPTOGRAPHY_ROTATION)
ELLE_LOG_COMPONENT("elle.cryptography.raw");
#endif

static const int max_block_size = 4096;
static const int buffer_sizes[] = {
  elle::cryptography::constants::stream_block_size,
  elle::cryptography::constants::stream_block_size + max_block_size
};

static
std::mutex&
_buffers_mutex()
{
  static std::mutex mutex;
  return mutex;
}

struct BufferDeleter
{
  void operator()(unsigned char* buf)
  {
    free(buf);
  }
};

static
std::pair<unsigned char*, unsigned char*>
buffers()
{
  using UPTR = std::unique_ptr<unsigned char, BufferDeleter>;
  static std::unordered_map<std::thread::id, std::pair<UPTR, UPTR>> map;
  std::unique_lock<std::mutex> ulock(_buffers_mutex());
  auto tid = std::this_thread::get_id();
  auto it = map.find(tid);
  if (it != map.end())
    return std::make_pair(it->second.first.get(), it->second.second.get());
  else
  {
    auto p = std::make_pair(UPTR((unsigned char*)malloc(buffer_sizes[0])),
                              UPTR((unsigned char*)malloc(buffer_sizes[1])));
    auto res = std::make_pair(p.first.get(), p.second.get());
    map[tid] = std::move(p);
    return res;
  }
}

//
// ---------- Asymmetric ------------------------------------------------------
//

namespace elle
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

      static
      elle::Buffer
      _apply(::EVP_PKEY_CTX* context,
             int (*function)(EVP_PKEY_CTX*,
                             unsigned char*,
                             size_t*,
                             const unsigned char*,
                             size_t),
             elle::ConstWeakBuffer const& input)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        ELLE_ASSERT(context != nullptr);
        ELLE_ASSERT_GTE(static_cast<uint32_t>(
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

      namespace asymmetric
      {
        /*----------.
        | Functions |
        `----------*/

        elle::Buffer
        encrypt(::EVP_PKEY* key,
                elle::ConstWeakBuffer const& plain,
                std::function<void (::EVP_PKEY_CTX*)> prolog,
                std::function<void (::EVP_PKEY_CTX*)> epilog)
        {
          // Make sure the cryptographic system is set up.
          cryptography::require();

          // Prepare the context.
          types::EVP_PKEY_CTX context(
            context::create(key, ::EVP_PKEY_encrypt_init));

          if (prolog)
            prolog(context.get());

          elle::Buffer code = _apply(context.get(),
                                     ::EVP_PKEY_encrypt,
                                     plain);

          if (epilog)
            epilog(context.get());

          return (code);
        }

        elle::Buffer
        decrypt(::EVP_PKEY* key,
                elle::ConstWeakBuffer const& code,
                std::function<void (::EVP_PKEY_CTX*)> prolog,
                std::function<void (::EVP_PKEY_CTX*)> epilog)
        {
          // Make sure the cryptographic system is set up.
          cryptography::require();

          // Prepare the context.
          types::EVP_PKEY_CTX context(
            context::create(key, ::EVP_PKEY_decrypt_init));

          if (prolog)
            prolog(context.get());

          elle::Buffer plain = _apply(context.get(), ::EVP_PKEY_decrypt, code);

          if (epilog)
            epilog(context.get());

          return (plain);
        }

        elle::Buffer
        sign(::EVP_PKEY* key,
             ::EVP_MD const* oneway,
             std::istream& plain,
             std::function<void (::EVP_MD_CTX*,
                                 ::EVP_PKEY_CTX*)> prolog,
             std::function<void (::EVP_MD_CTX*,
                                 ::EVP_PKEY_CTX*)> epilog)
        {
          // Make sure the cryptographic system is set up.
          cryptography::require();

          ELLE_ASSERT_NEQ(key, nullptr);

          ::EVP_MD_CTX context;
          ::EVP_PKEY_CTX* ctx = nullptr;

          // Initialize the signature context.
          ::EVP_MD_CTX_init(&context);

          if (EVP_DigestSignInit(&context, &ctx, oneway, NULL, key) <= 0)
            throw Error(
              elle::sprintf("unable to initialize the context for "
                            "signature: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ELLE_ASSERT(ctx != nullptr);
          if (prolog)
            prolog(&context, ctx);

          // Sign the plain's stream.
          unsigned char* _input = buffers().first;

          while (!plain.eof())
          {
            // Read the plain's input stream and put a block of data in a
            // temporary buffer.
            plain.read(reinterpret_cast<char*>(_input), constants::stream_block_size);
            if (plain.bad())
              throw Error(
                elle::sprintf("unable to read the plain's input stream: %s",
                              plain.rdstate()));

            // Update the signature context.
            if (::EVP_DigestSignUpdate(&context,
                                       _input,
                                       plain.gcount()) <= 0)
              throw Error(
                elle::sprintf("unable to apply the signature function: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));
          }

          // Finalize the signature.
          size_t size(0);

          // Determine the size of the signature.
          if (::EVP_DigestSignFinal(&context, NULL, &size) <= 0)
            throw Error(
              elle::sprintf("unable to determine the signature's finale "
                            "size: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ELLE_ASSERT_EQ(signed(size), ::EVP_PKEY_size(key));

          if (epilog)
            epilog(&context, ctx);

          // Allocate the output signature.
          elle::Buffer signature(size);

          if (::EVP_DigestSignFinal(&context,
                                    signature.mutable_contents(),
                                    &size) <= 0)
            throw Error(
              elle::sprintf("unable to finalize the signature process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Update the signature final size.
          signature.size(size);
          signature.shrink_to_fit();

          // Clean the context.
          if (::EVP_MD_CTX_cleanup(&context) <= 0)
            throw Error(
              elle::sprintf("unable to clean the signature context: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          return (signature);
        }

        bool
        verify(::EVP_PKEY* key,
               ::EVP_MD const* oneway,
               elle::ConstWeakBuffer const& signature,
               std::istream& plain,
               std::function<void (::EVP_MD_CTX*,
                                   ::EVP_PKEY_CTX*)> prolog,
               std::function<void (::EVP_MD_CTX*,
                                   ::EVP_PKEY_CTX*)> epilog)
        {
          // Make sure the cryptographic system is set up.
          cryptography::require();

          ELLE_ASSERT_NEQ(key, nullptr);

          ::EVP_MD_CTX context;
          ::EVP_PKEY_CTX* ctx = nullptr;

          // Initialize the verify context.
          ::EVP_MD_CTX_init(&context);

          if (EVP_DigestVerifyInit(&context, &ctx, oneway, NULL, key) <= 0)
            throw Error(
              elle::sprintf("unable to initialize the context for "
                            "verify: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ELLE_ASSERT(ctx != nullptr);
          if (prolog)
            prolog(&context, ctx);

          // Verify the signature's stream.
          unsigned char* _input = buffers().first;

          while (!plain.eof())
          {
            // Read the plain's input stream and put a block of data in a
            // temporary buffer.
            plain.read(reinterpret_cast<char*>(_input), constants::stream_block_size);
            if (plain.bad())
              throw Error(
                elle::sprintf("unable to read the plain's input stream: %s",
                              plain.rdstate()));

            // Update the verify context.
            if (::EVP_DigestVerifyUpdate(&context,
                                         _input,
                                         plain.gcount()) <= 0)
              throw Error(
                elle::sprintf("unable to apply the verify function: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));
          }

          if (epilog)
            epilog(&context, ctx);

          // Finalize the verification.
          int result =
            ::EVP_DigestVerifyFinal(&context,
                                    signature.contents(),
                                    signature.size());

          // Clean the context.
          if (::EVP_MD_CTX_cleanup(&context) <= 0)
            throw Error(
              elle::sprintf("unable to clean the verify context: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

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

        elle::Buffer
        agree(::EVP_PKEY* own,
              ::EVP_PKEY* peer,
              std::function<void (::EVP_PKEY_CTX*)> prolog,
              std::function<void (::EVP_PKEY_CTX*)> epilog)
        {
          // Prepare the context.
          types::EVP_PKEY_CTX context(
            context::create(own, ::EVP_PKEY_derive_init));

          if (prolog)
            prolog(context.get());

          // Set the peer key.
          if (::EVP_PKEY_derive_set_peer(context.get(), peer) <= 0)
            throw Error(
              elle::sprintf("unable to initialize the context for "
                            "derivation: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          size_t size;

          // Compute the shared key's future length.
          if (::EVP_PKEY_derive(context.get(), nullptr, &size) <= 0)
            throw Error(
              elle::sprintf("unable to compute the output size of the "
                            "shared key: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          elle::Buffer buffer(size);

          // Generate the shared key.
          if (::EVP_PKEY_derive(context.get(),
                                buffer.mutable_contents(),
                                &size) <= 0)
            throw Error(
              elle::sprintf("unable to generate the shared key: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Update the code size with the actual size of the generated data.
          buffer.size(size);
          buffer.shrink_to_fit();

          if (epilog)
            epilog(context.get());

          return (buffer);
        }

#if defined(ELLE_CRYPTOGRAPHY_ROTATION)
        elle::Buffer
        rotate(::EVP_PKEY* key,
               elle::ConstWeakBuffer const& seed,
               std::function<void (::EVP_PKEY_CTX*)> prolog,
               std::function<void (::EVP_PKEY_CTX*)> epilog)
        {
          ELLE_DUMP("seed: %s", seed);

          // Prepare the context.
          types::EVP_PKEY_CTX context(
            context::create(key, ::EVP_PKEY_sign_init));

          // Ensure the size of the seed equals the modulus.
          //
          // If the seed is too large, the algorithm would need to encrypt
          // it with a symmetric key etc. (as the seal() method does) which
          // would result in a future seed larger than the original.
          //
          // If it is too small, an attack could be performed against textbook
          // RSA, assuming it is the algorithm used in this case.
          if (static_cast<typename elle::Buffer::Size>(::EVP_PKEY_size(key)) !=
              seed.size())
            throw Error(
              elle::sprintf("unable to rotate a seed whose length does not "
                            "match the key's modulus: %s versus %s",
                            ::EVP_PKEY_size(key),
                            seed.size()));

          if (prolog)
            prolog(context.get());

          elle::Buffer buffer =
            _apply(context.get(), ::EVP_PKEY_sign, seed);

          if (epilog)
            epilog(context.get());

          // Make sure the seed does not grow over time.
          ELLE_ASSERT_EQ(seed.size(), buffer.size());

          return (buffer);
        }

        elle::Buffer
        unrotate(::EVP_PKEY* key,
                 elle::ConstWeakBuffer const& seed,
                 std::function<void (::EVP_PKEY_CTX*)> prolog,
                 std::function<void (::EVP_PKEY_CTX*)> epilog)
        {
          ELLE_DUMP("seed: %s", seed);

          // Prepare the context.
          types::EVP_PKEY_CTX context(
            context::create(key, ::EVP_PKEY_verify_recover_init));

          // As for the rotation mechanism, ensure the size of the seed
          // equals the modulus.
          if (static_cast<typename elle::Buffer::Size>(::EVP_PKEY_size(key)) !=
              seed.size())
            throw Error(
              elle::sprintf("unable to unrotate a seed whose length does not "
                            "match the key's modulus: %s versus %s",
                            ::EVP_PKEY_size(key),
                            seed.size()));

          if (prolog)
            prolog(context.get());

          elle::Buffer buffer =
            _apply(context.get(), ::EVP_PKEY_verify_recover, seed);

          if (epilog)
            epilog(context.get());

          // Make sure the unrotated seed has the same size as the original.
          ELLE_ASSERT_EQ(seed.size(), buffer.size());

          return (buffer);
        }
#endif
      }
    }
  }
}

//
// ---------- Symmetric -------------------------------------------------------
//

namespace elle
{
  namespace cryptography
  {
    namespace raw
    {
      namespace symmetric
      {
        /*----------.
        | Constants |
        `----------*/

        /// Define the magic which is embedded in every encrypted
        /// text so for the decryption process to know that the text
        /// has been salted.
        static char const magic[] = "Salted__";

        /*----------.
        | Functions |
        `----------*/

        void
        encipher(elle::ConstWeakBuffer const& secret,
                 ::EVP_CIPHER const* cipher,
                 ::EVP_MD const* oneway,
                 std::istream& plain,
                 std::ostream& code,
                 std::function<void (::EVP_CIPHER_CTX*)> prolog,
                 std::function<void (::EVP_CIPHER_CTX*)> epilog)
        {
          // Make sure the cryptographic system is set up.
          cryptography::require();

          // Generate a salt.
          unsigned char salt[PKCS5_SALT_LEN];

          if (::RAND_pseudo_bytes(salt, sizeof (salt)) <= 0)
            throw Error(elle::sprintf("unable to pseudo-randomly generate "
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

          if (::EVP_BytesToKey(cipher,
                               oneway,
                               salt,
                               secret.contents(),
                               secret.size(),
                               1,
                               key,
                               iv) > static_cast<int>(sizeof (key)))
            throw Error("the generated key size is too large");

          // Initialize the cipher context.
          ::EVP_CIPHER_CTX context;

          ::EVP_CIPHER_CTX_init(&context);

          ELLE_CRYPTOGRAPHY_FINALLY_ACTION_CLEANUP_CIPHER_CONTEXT(context);

          // Initialise the ciphering process.
          if (::EVP_EncryptInit_ex(&context,
                                   cipher,
                                   nullptr,
                                   key,
                                   iv) <= 0)
            throw Error(
              elle::sprintf("unable to initialize the encryption process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          if (prolog)
            prolog(&context);

          // Embed the magic and salt directly into the output code.
          code.write(magic, sizeof (magic) - 1);
          code.write(reinterpret_cast<const char*>(salt), sizeof (salt));
          if (!code.good())
            throw Error(
              elle::sprintf("unable to write the magic and salt to the code's "
                            "output stream: %s",
                            code.rdstate()));

          // Retreive the cipher-specific block size. This is the maximum size
          // that the algorithm can output on top of the encrypted input
          // plain text.
          int block_size = ::EVP_CIPHER_CTX_block_size(&context);
          ELLE_ASSERT_LTE(block_size, max_block_size);
          // Encrypt the input stream.
          auto bufs = buffers();
          unsigned char* _input = bufs.first;
          unsigned char* _output = bufs.second;
          while (!plain.eof())
          {
            // Read the plain's input stream and put a block of data in a
            // temporary buffer.
            plain.read(reinterpret_cast<char*>(_input), constants::stream_block_size);
            if (plain.bad())
              throw Error(
                elle::sprintf("unable to read the plain's input stream: %s",
                              plain.rdstate()));

            int size_update(0);

            // Encrypt the input buffer.
            if (::EVP_EncryptUpdate(&context,
                                    _output,
                                    &size_update,
                                    _input,
                                    plain.gcount()) <= 0)
            throw Error(
              elle::sprintf("unable to apply the encryption function: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

            // Write the output buffer to the code stream.
            code.write(reinterpret_cast<const char *>(_output),
                       size_update);
            if (!code.good())
              throw Error(
                elle::sprintf("unable to write the encrypted data to the "
                              "code's output stream: %s",
                              code.rdstate()));
          }

          if (epilog)
            epilog(&context);

          // Finalize the encryption process.
          int size_final(0);

          if (::EVP_EncryptFinal_ex(&context,
                                    _output,
                                    &size_final) <= 0)
            throw Error(
              elle::sprintf("unable to finalize the encryption process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Write the final output buffer to the code's output stream.
          code.write(reinterpret_cast<const char *>(_output),
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
        }

        void
        decipher(elle::ConstWeakBuffer const& secret,
                 ::EVP_CIPHER const* cipher,
                 ::EVP_MD const* oneway,
                 std::istream& code,
                 std::ostream& plain,
                 std::function<void (::EVP_CIPHER_CTX*)> prolog,
                 std::function<void (::EVP_CIPHER_CTX*)> epilog)
        {
          // Make sure the cryptographic system is set up.
          cryptography::require();

          // Check whether the code was produced with a salt.
          char _magic[sizeof (magic)];

          code.read(reinterpret_cast<char*>(_magic), sizeof (magic) - 1);
          if (!code.good())
            throw Error(
              elle::sprintf("unable to read the magic from the code's "
                            "input stream: %s",
                            code.rdstate()));

          if (::memcmp(_magic,
                       magic,
                       sizeof (magic) - 1) != 0)
            throw Error("the code was produced without any or an invalid "
                        "salt");

          // Copy the salt for the sack of clarity.
          unsigned char _salt[PKCS5_SALT_LEN];

          code.read(reinterpret_cast<char*>(_salt), sizeof (_salt));
          if (!code.good())
            throw Error(
              elle::sprintf("unable to read the salt from the code's "
                            "input stream: %s",
                            code.rdstate()));

          // Generate the key/IV tuple based on the salt.
          unsigned char key[EVP_MAX_KEY_LENGTH];
          unsigned char iv[EVP_MAX_IV_LENGTH];

          if (::EVP_BytesToKey(cipher,
                               oneway,
                               _salt,
                               secret.contents(),
                               secret.size(),
                               1,
                               key,
                               iv) > static_cast<int>(sizeof (key)))
            throw Error("the generated key size is too large");

          // Initialize the cipher context.
          ::EVP_CIPHER_CTX context;

          ::EVP_CIPHER_CTX_init(&context);

          ELLE_CRYPTOGRAPHY_FINALLY_ACTION_CLEANUP_CIPHER_CONTEXT(context);

          // Initialise the ciphering process.
          if (::EVP_DecryptInit_ex(&context,
                                   cipher,
                                   nullptr,
                                   key,
                                   iv) <= 0)
            throw Error(
              elle::sprintf("unable to initialize the decryption process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          if (prolog)
            prolog(&context);

          // Retreive the cipher-specific block size.
          int block_size = ::EVP_CIPHER_CTX_block_size(&context);
          ELLE_ASSERT_LTE(block_size, max_block_size);
          // Decipher the code's stream.
          auto bufs = buffers();
          unsigned char* _input = bufs.first;
          unsigned char* _output = bufs.second;

          while (!code.eof())
          {
            // Read the code's input stream and put a block of data in a
            // temporary buffer.
            code.read(reinterpret_cast<char*>(_input), constants::stream_block_size);
            if (code.bad())
              throw Error(
                elle::sprintf("unable to read the code's input stream: %s",
                              code.rdstate()));

            // Decrypt the input buffer.
            int size_update(0);

            if (::EVP_DecryptUpdate(&context,
                                    _output,
                                    &size_update,
                                    _input,
                                    code.gcount()) <= 0)
              throw Error(
                elle::sprintf("unable to apply the decryption function: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));

            // Write the output buffer to the plain stream.
            plain.write(reinterpret_cast<const char *>(_output),
                        size_update);
            if (!plain.good())
              throw Error(
                elle::sprintf("unable to write the decrypted data to the "
                              "plain's output stream: %s",
                              plain.rdstate()));
          }

          if (epilog)
            epilog(&context);

          // Finalize the deciphering process.
          int size_final(0);

          if (::EVP_DecryptFinal_ex(&context,
                                    _output,
                                    &size_final) <= 0)
            throw Error(
              elle::sprintf("unable to finalize the decryption process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Write the final output buffer to the plain's output stream.
          plain.write(reinterpret_cast<const char *>(_output),
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
        }
      }
    }
  }
}

//
// ---------- Hash ------------------------------------------------------------
//

namespace elle
{
  namespace cryptography
  {
    namespace raw
    {
      elle::Buffer
      hash(::EVP_MD const* oneway,
           std::istream& plain,
           std::function<void (::EVP_MD_CTX*)> prolog,
           std::function<void (::EVP_MD_CTX*)> epilog)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Initialise the context.
        ::EVP_MD_CTX context;

        ::EVP_MD_CTX_init(&context);

        ELLE_CRYPTOGRAPHY_FINALLY_ACTION_CLEANUP_DIGEST_CONTEXT(context);

        // Initialise the digest.
        if (::EVP_DigestInit_ex(&context, oneway, nullptr) <= 0)
          throw Error(
            elle::sprintf("unable to initialize the digest process: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        if (prolog)
          prolog(&context);

        // Hash the plain's stream.
        unsigned char* _input = buffers().first;

        while (!plain.eof())
        {
          // Read the plain's input stream and put a block of data in a
          // temporary buffer.
          plain.read(reinterpret_cast<char*>(_input), constants::stream_block_size);
          if (plain.bad())
            throw Error(
              elle::sprintf("unable to read the plain's input stream: %s",
                            plain.rdstate()));

          // Update the digest context.
          if (::EVP_DigestUpdate(&context,
                                 _input,
                                 plain.gcount()) <= 0)
            throw Error(
              elle::sprintf("unable to apply the digest function: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));
        }

        // Allocate the output digest.
        elle::Buffer digest(EVP_MD_size(oneway));

        if (epilog)
          epilog(&context);

        // Finalize the digest.
        unsigned int size(0);

        if (::EVP_DigestFinal_ex(&context,
                                 digest.mutable_contents(),
                                 &size) <= 0)
          throw Error(
            elle::sprintf("unable to finalize the digest process: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        // Update the digest final size.
        digest.size(size);
        digest.shrink_to_fit();

        // Clean the context.
        if (::EVP_MD_CTX_cleanup(&context) <= 0)
          throw Error(
            elle::sprintf("unable to clean the digest context: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        ELLE_CRYPTOGRAPHY_FINALLY_ABORT(context);

        return (digest);
      }
    }
  }
}

//
// ---------- HMAC ------------------------------------------------------------
//

namespace elle
{
  namespace cryptography
  {
    namespace raw
    {
      namespace hmac
      {
        elle::Buffer
        sign(::EVP_PKEY* key,
             ::EVP_MD const* oneway,
             std::istream& plain,
             std::function<void (::EVP_MD_CTX*)> prolog,
             std::function<void (::EVP_MD_CTX*)> epilog)
        {
          // Make sure the cryptographic system is set up.
          cryptography::require();

          // Initialise the context.
          ::EVP_MD_CTX context;

          ::EVP_MD_CTX_init(&context);

          ELLE_CRYPTOGRAPHY_FINALLY_ACTION_CLEANUP_DIGEST_CONTEXT(context);

          if (::EVP_DigestInit_ex(&context, oneway, NULL) <= 0)
            throw Error(
              elle::sprintf("unable to initialize the digest function: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          if (::EVP_DigestSignInit(&context, NULL, oneway, NULL, key) <= 0)
            throw Error(
              elle::sprintf("unable to initialize the HMAC process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          if (prolog)
            prolog(&context);

          // HMAC-sign the plain's stream.
          unsigned char* _input = buffers().first;

          while (!plain.eof())
          {
            // Read the plain's input stream and put a block of data in a
            // temporary buffer.
            plain.read(reinterpret_cast<char*>(_input), constants::stream_block_size);
            if (plain.bad())
              throw Error(
                elle::sprintf("unable to read the plain's input stream: %s",
                              plain.rdstate()));

            // Update the HMAC.
            if (::EVP_DigestSignUpdate(&context,
                                       _input,
                                       plain.gcount()) <= 0)
              throw Error(
                elle::sprintf("unable to apply the HMAC function: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));
          }

          if (epilog)
            epilog(&context);

          // Compute the output length.
          size_t size(0);

          if (::EVP_DigestSignFinal(&context,
                                    nullptr,
                                    &size) <= 0)
            throw Error(
              elle::sprintf("unable to compute the final HMAC size: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Finalize the digest.
          elle::Buffer digest(size);

          if (::EVP_DigestSignFinal(&context,
                                    digest.mutable_contents(),
                                    &size) <= 0)
            throw Error(
              elle::sprintf("unable to finalize the digest process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ELLE_ASSERT_EQ(digest.size(), size);

          // Update the digest final size.
          digest.size(size);
          digest.shrink_to_fit();

          // Clean the context.
          if (::EVP_MD_CTX_cleanup(&context) <= 0)
            throw Error(
              elle::sprintf("unable to clean the digest context: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ELLE_CRYPTOGRAPHY_FINALLY_ABORT(context);

          return (digest);
        }

        bool
        verify(::EVP_PKEY* key,
               ::EVP_MD const* oneway,
               elle::ConstWeakBuffer const& digest,
               std::istream& plain,
               std::function<void (::EVP_MD_CTX*)> prolog,
               std::function<void (::EVP_MD_CTX*)> epilog)
        {
          // Make sure the cryptographic system is set up.
          cryptography::require();

          // Initialise the context.
          ::EVP_MD_CTX context;

          ::EVP_MD_CTX_init(&context);

          ELLE_CRYPTOGRAPHY_FINALLY_ACTION_CLEANUP_DIGEST_CONTEXT(context);

          if (::EVP_DigestInit_ex(&context, oneway, NULL) <= 0)
            throw Error(
              elle::sprintf("unable to initialize the digest function: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          if (::EVP_DigestVerifyInit(&context, NULL, oneway, NULL, key) <= 0)
            throw Error(
              elle::sprintf("unable to initialize the HMAC process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          if (prolog)
            prolog(&context);

          // HMAC-verify the plain's stream against the digest.
          unsigned char* _input = buffers().first;

          while (!plain.eof())
          {
            // Read the plain's input stream and put a block of data in a
            // temporary buffer.
            plain.read(reinterpret_cast<char*>(_input), constants::stream_block_size);
            if (plain.bad())
              throw Error(
                elle::sprintf("unable to read the plain's input stream: %s",
                              plain.rdstate()));

            // Update the HMAC.
            if (::EVP_DigestVerifyUpdate(&context,
                                         _input,
                                         plain.gcount()) <= 0)
              throw Error(
                elle::sprintf("unable to apply the HMAC function: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));
          }

          if (epilog)
            epilog(&context);

          // Verify the context's data against the given digest.
          if (::EVP_DigestVerifyFinal(&context,
                                      digest.contents(),
                                      digest.size()) <= 0)
            return (false);

          // Clean the context.
          if (::EVP_MD_CTX_cleanup(&context) <= 0)
            throw Error(
              elle::sprintf("unable to clean the digest context: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ELLE_CRYPTOGRAPHY_FINALLY_ABORT(context);

          return (true);
        }
      }
    }
  }
}
