#ifndef INFINIT_CRYPTOGRAPHY_RSA_FWD_HH
# define INFINIT_CRYPTOGRAPHY_RSA_FWD_HH

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      class PrivateKey;
      class PublicKey;
      class KeyPair;
# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
      class Seed;
# endif
    }
  }
}

#endif
