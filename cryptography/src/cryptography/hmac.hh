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

      /// Sign a plain text using the given string as a key
      /// and return a HMAC digest.
      Digest
      sign(Plain const& plain,
           elle::String const& key,
           Oneway const oneway);
      /// HMAC any serializable data.
      template <typename T>
      Digest
      sign(T const& value,
           elle::String const& key,
           Oneway const oneway);
      /// Verify a HMAC digest.
      elle::Boolean
      verify(Digest const& digest,
             Plain const& plain,
             elle::String const& key,
             Oneway const oneway);
      /// Verify any serializable data.
      template <typename T>
      elle::Boolean
      verify(Digest const& digest,
             T const& value,
             elle::String const& key,
             Oneway const oneway);
    }
  }
}

# include <cryptography/hmac.hxx>

#endif
