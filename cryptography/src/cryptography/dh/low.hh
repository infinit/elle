#ifndef INFINIT_CRYPTOGRAPHY_DH_LOW_HH
# define INFINIT_CRYPTOGRAPHY_DH_LOW_HH

# include <openssl/dh.h>

namespace infinit
{
  namespace cryptography
  {
    namespace dh
    {
      namespace low
      {
        /*----------.
        | Functions |
        `----------*/

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
}

#endif
