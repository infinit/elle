#ifndef ELLE_CRYPTOGRAPHY_RSA_FWD_HH
# define ELLE_CRYPTOGRAPHY_RSA_FWD_HH

namespace elle
{
  namespace cryptography
  {
    namespace rsa
    {
      class PrivateKey;
      class PublicKey;
      class KeyPair;
# if defined(ELLE_CRYPTOGRAPHY_ROTATION)
      class Seed;
# endif
    }
  }
}

#endif
