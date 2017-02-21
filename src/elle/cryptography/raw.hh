#ifndef ELLE_CRYPTOGRAPHY_RAW_HH
# define ELLE_CRYPTOGRAPHY_RAW_HH

# include <elle/cryptography/fwd.hh>
# include <elle/cryptography/Cipher.hh>
# include <elle/cryptography/Oneway.hh>

# include <elle/types.hh>
# include <elle/fwd.hh>

# include <openssl/evp.h>

# include <memory>

//
// ---------- Asymmetric ------------------------------------------------------
//

namespace elle
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

        /// Encrypt the given plain with the provided encryption key.
        elle::Buffer
        encrypt(::EVP_PKEY* key,
                elle::ConstWeakBuffer const& plain,
                std::function<void (::EVP_PKEY_CTX*)> prolog = nullptr,
                std::function<void (::EVP_PKEY_CTX*)> epilog = nullptr);
        /// Decrypt the code with the provided key and function.
        elle::Buffer
        decrypt(::EVP_PKEY* key,
                elle::ConstWeakBuffer const& code,
                std::function<void (::EVP_PKEY_CTX*)> prolog = nullptr,
                std::function<void (::EVP_PKEY_CTX*)> epilog = nullptr);
        /// Sign the given plain text.
        elle::Buffer
        sign(::EVP_PKEY* key,
             ::EVP_MD const* oneway,
             std::istream& plain,
             std::function<void (::EVP_MD_CTX*,
                                 ::EVP_PKEY_CTX*)> prolog = nullptr,
             std::function<void (::EVP_MD_CTX*,
                                 ::EVP_PKEY_CTX*)> epilog = nullptr);
        /// Return true if the signature is valid according to the given plain.
        bool
        verify(::EVP_PKEY* key,
               ::EVP_MD const* oneway,
               elle::ConstWeakBuffer const& signature,
               std::istream& plain,
               std::function<void (::EVP_MD_CTX*,
                                   ::EVP_PKEY_CTX*)> prolog = nullptr,
               std::function<void (::EVP_MD_CTX*,
                                   ::EVP_PKEY_CTX*)> epilog = nullptr);
        /// Agree on a shared key between two key pairs: between a one's private
        /// key and a peer's public key.
        elle::Buffer
        agree(::EVP_PKEY* own,
              ::EVP_PKEY* peer,
              std::function<void (::EVP_PKEY_CTX*)> prolog = nullptr,
              std::function<void (::EVP_PKEY_CTX*)> epilog = nullptr);
#if defined(ELLE_CRYPTOGRAPHY_ROTATION)
        /// Rotate the given seed with the key.
        elle::Buffer
        rotate(::EVP_PKEY* key,
               elle::ConstWeakBuffer const& seed,
               std::function<void (::EVP_PKEY_CTX*)> prolog = nullptr,
               std::function<void (::EVP_PKEY_CTX*)> epilog = nullptr);
        /// Unrotate the given seed according to the key.
        elle::Buffer
        unrotate(::EVP_PKEY* key,
                 elle::ConstWeakBuffer const& seed,
                 std::function<void (::EVP_PKEY_CTX*)> prolog = nullptr,
                 std::function<void (::EVP_PKEY_CTX*)> epilog = nullptr);
# endif
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
      /// Contain the operations related to symmetric algorithms.
      namespace symmetric
      {
        /// Encipher the plain text according to the given secret and functions.
        void
        encipher(elle::ConstWeakBuffer const& secret,
                 ::EVP_CIPHER const* cipher,
                 ::EVP_MD const* oneway,
                 std::istream& plain,
                 std::ostream& code,
                 std::function<void (::EVP_CIPHER_CTX*)> prolog = nullptr,
                 std::function<void (::EVP_CIPHER_CTX*)> epilog = nullptr);
        /// Decipher the cipher text according to the given secret and
        /// functions.
        void
        decipher(elle::ConstWeakBuffer const& secret,
                 ::EVP_CIPHER const* cipher,
                 ::EVP_MD const* oneway,
                 std::istream& code,
                 std::ostream& plain,
                 std::function<void (::EVP_CIPHER_CTX*)> prolog = nullptr,
                 std::function<void (::EVP_CIPHER_CTX*)> epilog = nullptr);
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
      /// Hash the given plain text and return a message digest.
      elle::Buffer
      hash(::EVP_MD const* oneway,
           std::istream& plain,
           std::function<void (::EVP_MD_CTX*)> prolog = nullptr,
           std::function<void (::EVP_MD_CTX*)> epilog = nullptr);
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
        /// HMAC the given plain text using a key and digest function.
        elle::Buffer
        sign(::EVP_PKEY* key,
             ::EVP_MD const* oneway,
             std::istream& plain,
             std::function<void (::EVP_MD_CTX*)> prolog = nullptr,
             std::function<void (::EVP_MD_CTX*)> epilog = nullptr);
        /// Verify a HMAC digest.
        bool
        verify(::EVP_PKEY* key,
               ::EVP_MD const* oneway,
               elle::ConstWeakBuffer const& digest,
               std::istream& plain,
               std::function<void (::EVP_MD_CTX*)> prolog = nullptr,
               std::function<void (::EVP_MD_CTX*)> epilog = nullptr);
      }
    }
  }
}

#endif
