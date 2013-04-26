#ifndef INFINIT_CRYPTOGRAPHY_FWD_HH
# define INFINIT_CRYPTOGRAPHY_FWD_HH

namespace infinit
{
  namespace cryptography
  {
    class Exception;
    class Input;
    class KeyPair;
    class Output;
    class PrivateKey;
    namespace privatekey
    {
      class Interface;
    }
    class PublicKey;
    namespace publickey
    {
      class Interface;
    }
    class SecretKey;
# if defined(ELLE_CRYPTOGRAPHY_ROTATION)
    namespace seed
    {
      class Interface;
    }
    class Seed;
# endif
  }
}

# include <cryptography/Code.hh>
# include <cryptography/cipher.hh>
# include <cryptography/Clear.hh>
# include <cryptography/Cryptosystem.hh>
# include <cryptography/Digest.hh>
# include <cryptography/Plain.hh>
# include <cryptography/Signature.hh>
# include <cryptography/rsa/fwd.hh>

#endif
