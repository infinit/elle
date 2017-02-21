#ifndef ELLE_CRYPTOGRAPHY_ENVELOPE_HH
# define ELLE_CRYPTOGRAPHY_ENVELOPE_HH

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
    /// Contains high-level cryptographic operation known as envelope
    /// sealing/opening which concretely are encryption/decryption processes
    /// to handle larger amount of data than the asymmetric keys support.
    namespace envelope
    {
      /*----------.
      | Functions |
      `----------*/

      /// Seal the given plain with the provided encryption key.
      void
      seal(::EVP_PKEY* key,
           ::EVP_CIPHER const* cipher,
           std::istream& plain,
           std::ostream& code);
      /// Open the envelope with the provided key.
      void
      open(::EVP_PKEY* key,
           ::EVP_CIPHER const* cipher,
           std::istream& code,
           std::ostream& plain);
    }
  }
}

#endif
