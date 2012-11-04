#ifndef ELLE_CRYPTOGRAPHY_ONEWAY_HH
# define ELLE_CRYPTOGRAPHY_ONEWAY_HH

# include <elle/types.hh>

# include <elle/cryptography/fwd.hh>

namespace elle
{
  namespace cryptography
  {
    /// Contains everything related to hashing.
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
      ///
      /// Note that an rvalue reference is used here in order to catch all
      /// the plain-based calls.
      Digest
      hash(Plain const& plain,
           Algorithm algorithm);
      Digest
      hash(Plain&& plain,
           Algorithm algorithm);
      /// Hash anything serializable and return a digest.
      template <typename T>
      Digest
      hash(T const& value,
           Algorithm algorithm);
    }
  }
}

# include <elle/cryptography/oneway.hxx>

#endif
