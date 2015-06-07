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
    namespace hmac
    {
      /*----------.
      | Functions |
      `----------*/

      /// HMAC any serializable data, including plain texts i.e Plain
      /// with a string-based key.
      template <typename T = Plain>
      Digest
      sign(T const& value,
           elle::String const& key,
           Oneway const oneway);
      /// HMAC with a private key, no matter the algorithm. Example:
      ///
      ///   rsa::PrivateKey k;
      ///
      ///   hmac::sign(value, k, Oneway::sha1);
      template <typename T = Plain,
                typename K>
      Digest
      sign(T const& value,
           K const& key,
           Oneway const oneway);
      /// Verify any serializable data, include plain texts with a
      /// string-based key.
      template <typename T = Plain>
      elle::Boolean
      verify(Digest const& digest,
             T const& value,
             elle::String const& key,
             Oneway const oneway);
      /// Verify a HMAC signature with the public key associated with
      /// the private key used to sign.
      template <typename T = Plain,
                typename K>
      elle::Boolean
      verify(Digest const& digest,
             T const& value,
             K const& key,
             Oneway const oneway);
    }
  }
}

# include <cryptography/hmac.hxx>

#endif
