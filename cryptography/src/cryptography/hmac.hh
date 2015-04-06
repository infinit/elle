#ifndef INFINIT_CRYPTOGRAPHY_HMAC_HH
# define INFINIT_CRYPTOGRAPHY_HMAC_HH

# include <cryptography/fwd.hh>
# include <cryptography/Oneway.hh>
# include <cryptography/Digest.hh>

# include <elle/types.hh>

# include <openssl/evp.h>

# include <iosfwd>

namespace infinit
{
  namespace cryptography
  {
    /*----------.
    | Functions |
    `----------*/

    /// HMAC plain text using a key and return a digest.
    Digest
    hmac(Plain const& plain,
         Digest const& key,
         Oneway oneway);
    /// HMAC anything serializable using a key and return a digest.
    template <typename T>
    Digest
    hmac(T const& value,
         Digest const& key,
         Oneway oneway);
  }
}

# include <cryptography/hmac.hxx>

#endif
