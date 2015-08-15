#ifndef INFINIT_CRYPTOGRAPHY_RAW_HH
# define INFINIT_CRYPTOGRAPHY_RAW_HH

# include <cryptography/fwd.hh>
# include <cryptography/Cipher.hh>
# include <cryptography/Oneway.hh>

# include <elle/types.hh>
# include <elle/fwd.hh>

# include <openssl/evp.h>

# include <memory>

//
// ---------- Asymmetric ------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace raw
    {
      /// Contain the operations related to asymmetric cryptosystems.
      ///
      /// Note that these functions do not operate on streams because
      /// such operations are limited due to the nature of asymmetric
      /// cryptosystems.
      namespace asymmetric
      {
        /*----------.
        | Functions |
        `----------*/

        /// Encrypt the given plain with the provided encryption context and
        /// function.
        ///
        /// Note that a padding size is provided, in bits, representing the
        /// number of bits taken by the padding in the output code.
        elle::Buffer
        encrypt(elle::ConstWeakBuffer const& plain,
                ::EVP_PKEY_CTX* context,
                int (*function)(EVP_PKEY_CTX*,
                                unsigned char*,
                                size_t*,
                                const unsigned char*,
                                size_t));
        /// Decrypt the code with the provided context and function.
        elle::Buffer
        decrypt(elle::ConstWeakBuffer const& code,
                ::EVP_PKEY_CTX* context,
                int (*function)(EVP_PKEY_CTX*,
                                unsigned char*,
                                size_t*,
                                const unsigned char*,
                                size_t));
#if !defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        /// Sign the given plain text.
        elle::Buffer
        sign(::EVP_PKEY* key,
             ::EVP_MD const* oneway,
             std::istream& plain,
             std::function<void (::EVP_PKEY_CTX*)> configure =
               [] (::EVP_PKEY_CTX* context) -> void {});
        /// Return true if the signature is valid according to the given plain.
        elle::Boolean
        verify(::EVP_PKEY* key,
               ::EVP_MD const* oneway,
               elle::ConstWeakBuffer const& signature,
               std::istream& plain,
               std::function<void (::EVP_PKEY_CTX*)> configure =
                 [] (::EVP_PKEY_CTX* context) -> void {});
#endif
        /// Agree on a shared key between two key pairs: between a one's private
        /// key and a peer's public key.
        elle::Buffer
        agree(::EVP_PKEY_CTX* context,
              ::EVP_PKEY* peer);
        /// Rotate the given seed with the context and according to the
        /// given function.
        elle::Buffer
        rotate(elle::ConstWeakBuffer const& seed,
               ::EVP_PKEY_CTX* context,
               int (*function)(EVP_PKEY_CTX*,
                               unsigned char*,
                               size_t*,
                               const unsigned char*,
                               size_t));
        /// Unrotate the given seed according to the context and function.
        elle::Buffer
        unrotate(elle::ConstWeakBuffer const& seed,
                 ::EVP_PKEY_CTX* context,
                 int (*function)(EVP_PKEY_CTX*,
                                 unsigned char*,
                                 size_t*,
                                 const unsigned char*,
                                 size_t));
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
    namespace raw
    {
      /// Contain the operations related to symmetric algorithms.
      namespace symmetric
      {
        /// Encipher the plain text according to the given secret and functions.
        void
        encipher(std::istream& input,
                 std::ostream& output,
                 elle::ConstWeakBuffer const& secret,
                 ::EVP_CIPHER const* function_cipher,
                 ::EVP_MD const* function_oneway);
        /// Decipher the cipher text according to the given secret and
        /// functions.
        void
        decipher(std::istream& code,
                 std::ostream& plain,
                 elle::ConstWeakBuffer const& secret,
                 ::EVP_CIPHER const* function_cipher,
                 ::EVP_MD const* function_oneway);
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
    namespace raw
    {
      /// Hash the given plain text and return a message digest.
      elle::Buffer
      hash(std::istream& plain,
           ::EVP_MD const* function);
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
    namespace raw
    {
      namespace hmac
      {
        /// HMAC the given plain text using a key and digest function.
        elle::Buffer
        sign(std::istream& plain,
             ::EVP_PKEY* key,
             ::EVP_MD const* function);
        /// Verify a HMAC digest.
        elle::Boolean
        verify(elle::ConstWeakBuffer const& digest,
               std::istream& plain,
               ::EVP_PKEY* key,
               ::EVP_MD const* function);
      }
    }
  }
}

#endif
