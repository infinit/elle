#ifndef INFINIT_CRYPTOGRAPHY_FWD_HH
# define INFINIT_CRYPTOGRAPHY_FWD_HH

namespace infinit
{
  namespace cryptography
  {
    class Exception;
    class KeyPair;
    class Input;
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
    class Seed;
  }
}

# include <cryptography/Code.hh>
# include <cryptography/cipher.hh>
# include <cryptography/Clear.hh>
# include <cryptography/Cryptosystem.hh>
# include <cryptography/Digest.hh>
# include <cryptography/Plain.hh>
# include <cryptography/Signature.hh>

#endif
