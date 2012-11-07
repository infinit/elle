#ifndef INFINIT_CRYPTOGRAPHY_ONEWAY_HH
# define INFINIT_CRYPTOGRAPHY_ONEWAY_HH

# include <elle/types.hh>

# include <cryptography/fwd.hh>
# include <cryptography/Digest.hh>

namespace infinit
{
  namespace cryptography
  {
    /// Contain everything related to hashing.
    namespace oneway
    {
      /*-------------.
      | Enumerations |
      `-------------*/
      enum class Algorithm
      {
        md5,
        sha,
        sha1,
        sha224,
        sha256,
        sha384,
        sha512
      };

      /*---------------.
      | Static Methods |
      `---------------*/
      /// Hash a plain text and return a digest.
      Digest
      hash(Plain const& plain,
           Algorithm algorithm);
      /// Hash anything serializable and return a digest.
      template <typename T>
      Digest
      hash(T const& value,
           Algorithm algorithm);
    }
  }
}

# include <cryptography/oneway.hxx>

#endif
