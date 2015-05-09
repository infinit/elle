#ifndef INFINIT_CRYPTOGRAPHY_EVP_HH
# define INFINIT_CRYPTOGRAPHY_EVP_HH

# include <cryptography/fwd.hh>
# include <cryptography/Code.hh>
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
    namespace evp
    {
      /// Contain the operations related to asymmetric cryptosystems.
      namespace asymmetric
      {
        /*----------.
        | Functions |
        `----------*/

        /// Apply the given cryptographic function/context on the buffer.
        elle::Buffer
        apply(elle::ConstWeakBuffer const& input,
              ::EVP_PKEY_CTX* context,
              int (*function)(EVP_PKEY_CTX*,
                              unsigned char*,
                              size_t*,
                              const unsigned char*,
                              size_t));
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
                                size_t),
                ::EVP_CIPHER const* cipher,
                ::EVP_MD const* oneway,
                elle::Natural32 const padding_size);
        /// Decrypt the code with the provided context and function.
        elle::Buffer
        decrypt(elle::ConstWeakBuffer const& code,
                ::EVP_PKEY_CTX* context,
                int (*function)(EVP_PKEY_CTX*,
                                unsigned char*,
                                size_t*,
                                const unsigned char*,
                                size_t));
        /// Sign the given digest.
        elle::Buffer
        sign(elle::ConstWeakBuffer const& digest,
             ::EVP_PKEY_CTX* context,
             int (*function)(EVP_PKEY_CTX*,
                             unsigned char*,
                             size_t*,
                             const unsigned char*,
                             size_t));
        /// Return true if the signature is valid according to the given digest.
        elle::Boolean
        verify(elle::ConstWeakBuffer const& signature,
               elle::ConstWeakBuffer const& digest,
               ::EVP_PKEY_CTX* context,
               int (*function)(EVP_PKEY_CTX*,
                               const unsigned char*,
                               size_t,
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
    namespace evp
    {
      /// Contain the operations related to symmetric algorithms.
      namespace symmetric
      {
        /// Encrypt the plain text according to the given secret and functions.
        elle::Buffer
        encrypt(elle::ConstWeakBuffer const& plain,
                elle::ConstWeakBuffer const& secret,
                ::EVP_CIPHER const* function_cipher,
                ::EVP_MD const* function_oneway);
        /// Decrypt the ciphered text according to the given secret and
        /// functions.
        elle::Buffer
        decrypt(elle::ConstWeakBuffer const& code,
                elle::ConstWeakBuffer const& secret,
                ::EVP_CIPHER const* function_cipher,
                ::EVP_MD const* function_oneway);
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
        /// Hash the given plain text with the message digest function.
        elle::Buffer
        hash(elle::ConstWeakBuffer const& plain,
             ::EVP_MD const* function);
        /// HMAC the given plain text using a key and digest function.
        elle::Buffer
        hmac(elle::ConstWeakBuffer const& plain,
             ::EVP_PKEY* key,
             ::EVP_MD const* function);
      }
    }
  }
}

#endif
