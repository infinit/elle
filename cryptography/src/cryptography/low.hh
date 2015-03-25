#ifndef INFINIT_CRYPTOGRAPHY_LOW_HH
# define INFINIT_CRYPTOGRAPHY_LOW_HH

# include <openssl/rsa.h>

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

      // Return a copy of the given private key that has been transformed
      // into a RSA public key.
      ::RSA*
      RSA_priv2pub(::RSA* private_key);
      // Duplicate a RSA key.
      ::RSA*
      RSA_dup(::RSA* key);
    }
  }
}

#endif
