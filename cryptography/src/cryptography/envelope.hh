#ifndef INFINIT_CRYPTOGRAPHY_ENVELOPE_HH
# define INFINIT_CRYPTOGRAPHY_ENVELOPE_HH

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
    /// Contains high-level cryptographic operation known as envelope
    /// sealing/opening which concretely are encryption/decryption processes
    /// to handle larger amount of data than the asymmetric keys support.
    ///
    /// Note that the asymmetric cryptosystems do not provide stream-based
    /// cryptographic methods (encryption, signature etc.) unlike symmetric
    /// and oneway algorithms. Consider creating the envelope yourself should
    /// you want to encrypt a large amount of data: generate a temporary
    /// key, encrypt the data with a symmetric algorithm (e.g AES) and encrypt
    /// the temporary key with the asymmetric algorithm.
    namespace envelope
    {
      /*----------.
      | Functions |
      `----------*/

      /// Seal the given plain with the provided encryption context and
      /// function.
      ///
      /// Note that a padding size is provided, in bits, representing the
      /// number of bits taken by the padding in the output code.
      void
      seal(::EVP_PKEY* key,
           ::EVP_CIPHER const* cipher,
           std::istream& plain,
           std::ostream& code);
      /// Open the envelope with the provided context and function.
      void
      open(::EVP_PKEY* key,
           ::EVP_CIPHER const* cipher,
           std::istream& code,
           std::ostream& plain);
    }
  }
}

#endif
