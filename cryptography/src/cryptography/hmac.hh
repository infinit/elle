#ifndef INFINIT_CRYPTOGRAPHY_HMAC_HH
# define INFINIT_CRYPTOGRAPHY_HMAC_HH

# include <cryptography/fwd.hh>
# include <cryptography/Oneway.hh>

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
      /// Sign a buffer with a string-based key.
      elle::Buffer
      sign(elle::ConstWeakBuffer const& plain,
           elle::String const& key,
           Oneway const oneway);
      /// Verify a buffer-based HMAC with string-based key.
      elle::Boolean
      verify(elle::ConstWeakBuffer const& digest,
             elle::ConstWeakBuffer const& plain,
             elle::String const& key,
             Oneway const oneway);
      /// Sign a buffer with an asymmetric key.
      template <typename K>
      elle::Buffer
      sign(elle::ConstWeakBuffer const& plain,
           K const& key,
           Oneway const oneway);
      /// Verify a buffer-based HMAC with an asymmetric key.
      template <typename K>
      elle::Boolean
      verify(elle::ConstWeakBuffer const& digest,
             elle::ConstWeakBuffer const& plain,
             K const& key,
             Oneway const oneway);
# endif
      /// Sign a stream with a string-based key.
      elle::Buffer
      sign(std::istream& plain,
           elle::String const& key,
           Oneway const oneway);
      /// Verify a stream-based HMAC with a string-based key.
      elle::Boolean
      verify(elle::ConstWeakBuffer const& digest,
             std::istream& plain,
             elle::String const& key,
             Oneway const oneway);
      /// Sign a stream with an asymmetric key.
      template <typename K>
      elle::Buffer
      sign(std::istream& plain,
           K const& key,
           Oneway const oneway);
      /// Verify a stream-based HMAC with an asymmetric key.
      template <typename K>
      elle::Boolean
      verify(elle::ConstWeakBuffer const& digest,
             std::istream& plain,
             K const& key,
             Oneway const oneway);

# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
    /*-------.
    | Legacy |
    `-------*/

      template <typename T = Plain>
      Digest
      sign(T const& value,
           elle::ConstWeakBuffer const& key,
           Oneway const oneway);
# endif
    }
  }
}

# include <cryptography/hmac.hxx>

#endif
