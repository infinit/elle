#ifndef INFINIT_CRYPTOGRAPHY_RSA_HMAC_HH
# define INFINIT_CRYPTOGRAPHY_RSA_HMAC_HH

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
    namespace rsa
    {
      namespace hmac
      {
        /*----------.
        | Functions |
        `----------*/

        /// Sign a plain text using the RSA key and return a HMAC digest.
        Digest
        sign(Plain const& plain,
             PrivateKey const& k,
             Oneway const oneway);
        /// HMAC any serializable data.
        template <typename T>
        Digest
        sign(T const& value,
             PrivateKey const& k,
             Oneway const oneway);
        /// Verify a HMAC digest.
        elle::Boolean
        verify(Digest const& digest,
               Plain const& plain,
               PublicKey const& K,
               Oneway const oneway);
        /// Verify any serializable data.
        template <typename T>
        elle::Boolean
        verify(Digest const& digest,
               T const& value,
               PublicKey const& K,
               Oneway const oneway);
      }
    }
  }
}

# include <cryptography/rsa/hmac.hxx>

#endif
