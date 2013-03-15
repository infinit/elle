#ifndef INFINIT_CRYPTOGRAPHY_RSA_KEYPAIR_HH
# define INFINIT_CRYPTOGRAPHY_RSA_KEYPAIR_HH

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
        | Functions |
        `----------*/

        /// Return a pair composed of the public and private key of a
        /// freshly generated RSA key pair.
        ///
        /// Note that the length is in bits.
        std::pair<PublicKey, PrivateKey>
        generate(elle::Natural32 const length);
      }
    }
  }
}

#endif
