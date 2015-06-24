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
# if !defined(INFINIT_CRYPTOGRAPHY_LEGACY)
      Digest
      sign(Plain const& plain,
           elle::String const& key,
           Oneway const oneway);
      template <typename K>
      Digest
      sign(Plain const& plain,
           K const& key,
           Oneway const oneway);
      elle::Boolean
      verify(Digest const& digest,
             Plain const& plain,
             elle::String const& key,
             Oneway const oneway);
      template <typename K>
      elle::Boolean
      verify(Digest const& digest,
             Plain const& plain,
             K const& key,
             Oneway const oneway);
# endif

# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
      /*-------.
      | Legacy |
      `-------*/

      template <typename T = Plain>
      Digest
      sign(T const& value,
           elle::String const& key,
           Oneway const oneway);
      template <typename T = Plain,
                typename K>
      Digest
      sign(T const& value,
           K const& key,
           Oneway const oneway);
      template <typename T = Plain>
      elle::Boolean
      verify(Digest const& digest,
             T const& value,
             elle::String const& key,
             Oneway const oneway);
      template <typename T = Plain,
                typename K>
      elle::Boolean
      verify(Digest const& digest,
             T const& value,
             K const& key,
             Oneway const oneway);
# endif
    }
  }
}

# include <cryptography/hmac.hxx>

#endif
