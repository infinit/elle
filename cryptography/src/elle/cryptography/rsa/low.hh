#ifndef ELLE_CRYPTOGRAPHY_RSA_LOW_HH
# define ELLE_CRYPTOGRAPHY_RSA_LOW_HH

# include <elle/cryptography/fwd.hh>

namespace elle
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace low
      {
        /*----------.
        | Functions |
        `----------*/

        /// Return a copy of the given private key that has been transformed
        /// into a RSA public key.
        ::RSA*
        RSA_priv2pub(::RSA* private_key);
        /// Duplicate a RSA key.
        ::RSA*
        RSA_dup(::RSA* key);
      }
    }
  }
}

#endif
