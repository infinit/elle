#ifndef INFINIT_CRYPTOGRAPHY_RSA_KEYPAIR_HH
# define INFINIT_CRYPTOGRAPHY_RSA_KEYPAIR_HH

# include <cryptography/fwd.hh>
# include <cryptography/rsa/PublicKey.hh>
# include <cryptography/rsa/PrivateKey.hh>

# include <elle/types.hh>

# include <utility>

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace keypair
      {
        /*----------.
        | Constants |
        `----------*/
        /// The default length for secure RSA key pairs.
        static elle::Natural32 const default_length = 8192;

        /*----------.
        | Functions |
        `----------*/

        /// Return a pair composed of the public and private key of a
        /// freshly generated RSA key pair.
        ///
        /// Note that the length is in bits.
        std::pair<PublicKey, PrivateKey>
        generate(elle::Natural32 const length = default_length);
# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        /// Generate a key pair in a deterministic way (i.e deduce) based on
        /// the given seed.
        std::pair<PublicKey, PrivateKey>
        deduce(cryptography::seed::Interface const& seed);
# endif
        /// Construct a pair of keys based on the given EVP structure.
        ///
        /// Note that the ownership on the given EVP structure is transferred
        /// to the function.
        std::pair<PublicKey, PrivateKey>
        construct(::EVP_PKEY* key);
      }
    }
  }
}

#endif
