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

      /// Define the oneway algorithm.
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

      /*----------.
      | Functions |
      `----------*/

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

    /*----------.
    | Operators |
    `----------*/

    // XXX operator << for Algorithm
  }
}

# include <cryptography/oneway.hxx>

#endif
