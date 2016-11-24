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

      /// Sign a buffer with a string-based key.
      elle::Buffer
      sign(elle::ConstWeakBuffer const& plain,
           std::string const& key,
           Oneway const oneway);
      /// Verify a buffer-based HMAC with string-based key.
      bool
      verify(elle::ConstWeakBuffer const& digest,
             elle::ConstWeakBuffer const& plain,
             std::string const& key,
             Oneway const oneway);
      /// Sign a buffer with an asymmetric key.
      template <typename K>
      elle::Buffer
      sign(elle::ConstWeakBuffer const& plain,
           K const& key,
           Oneway const oneway);
      /// Verify a buffer-based HMAC with an asymmetric key.
      template <typename K>
      bool
      verify(elle::ConstWeakBuffer const& digest,
             elle::ConstWeakBuffer const& plain,
             K const& key,
             Oneway const oneway);
      /// Sign a stream with a string-based key.
      elle::Buffer
      sign(std::istream& plain,
           std::string const& key,
           Oneway const oneway);
      /// Verify a stream-based HMAC with a string-based key.
      bool
      verify(elle::ConstWeakBuffer const& digest,
             std::istream& plain,
             std::string const& key,
             Oneway const oneway);
      /// Sign a stream with an asymmetric key.
      template <typename K>
      elle::Buffer
      sign(std::istream& plain,
           K const& key,
           Oneway const oneway);
      /// Verify a stream-based HMAC with an asymmetric key.
      template <typename K>
      bool
      verify(elle::ConstWeakBuffer const& digest,
             std::istream& plain,
             K const& key,
             Oneway const oneway);
    }
  }
}

# include <cryptography/hmac.hxx>

#endif
