#ifndef INFINIT_CRYPTOGRAPHY_LOW_HH
# define INFINIT_CRYPTOGRAPHY_LOW_HH

# include <openssl/dsa.h>
# include <openssl/dh.h>

namespace infinit
{
  namespace cryptography
  {
    /// Contain functionalities missing in the OpenSSL library.
    namespace low
    {
      /*----------.
      | Functions |
      `----------*/

      /// Return a copy of the given private key that has been transformed
      /// into a DSA public key.
      ::DSA*
      DSA_priv2pub(::DSA* private_key);
      /// Duplicate a DSA key.
      ::DSA*
      DSA_dup(::DSA* key);
      /// Return a copy of the given private key that has been transformed
      /// into a DH public key.
      ::DH*
      DH_priv2pub(::DH* private_key);
      /// Duplicate a DH key.
      ::DH*
      DH_dup(::DH* key);
    }
  }
}

#endif
