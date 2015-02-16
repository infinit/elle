#ifndef INFINIT_CRYPTOGRAPHY_ONEWAY_HH
# define INFINIT_CRYPTOGRAPHY_ONEWAY_HH

# include <cryptography/fwd.hh>
# include <cryptography/Digest.hh>

# include <elle/types.hh>

# include <openssl/evp.h>

# include <iosfwd>

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

      /// Resolve a algorithm name into an EVP function pointer.
      ::EVP_MD const*
      resolve(Algorithm const name);
      /// Hash a plain text and return a digest.
      Digest
      hash(Plain const& plain,
           Algorithm algorithm);
      /// Hash anything serializable and return a digest.
      template <typename T>
      Digest
      hash(T const& value,
           Algorithm algorithm);
      /// HMAC plain text using a key and return a digest.
      Digest
      hmac(Plain const& plain,
           Digest const& key,
           Algorithm algorithm);
      /// HMAC anything serializable using a key and return a digest.
      template <typename T>
      Digest
      hmac(T const& plain,
           Digest const& key,
           Algorithm algorithm);

      /*----------.
      | Operators |
      `----------*/

      std::ostream&
      operator <<(std::ostream& stream,
                  Algorithm const algorithm);
    }
  }
}

# include <cryptography/oneway.hxx>

#endif
