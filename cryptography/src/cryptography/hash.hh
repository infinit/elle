#ifndef INFINIT_CRYPTOGRAPHY_HASH_HH
# define INFINIT_CRYPTOGRAPHY_HASH_HH

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

    /// Hash a plain text and return a digest.
    Digest
    hash(Plain const& plain,
         Oneway oneway);
    /// Hash anything serializable and return a digest.
    template <typename T>
    Digest
    hash(T const& value,
         Oneway oneway);
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

# include <cryptography/hash.hxx>

#endif
