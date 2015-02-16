#ifndef INFINIT_CRYPTOGRAPHY_FWD_HH
# define INFINIT_CRYPTOGRAPHY_FWD_HH

namespace infinit
{
  namespace cryptography
  {
    enum class Cryptosystem;
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
    /// Represent a plain text for cryptographic operations.
    typedef Input Plain;
    /// Represent a decrypted text, both trought symmetric and asymmetric
    /// cryptosystems.
    typedef Output Clear;
    /// Represent an encrypted text, asymmetrically or symmetrically.
    typedef Output Code;
    /// Represent an asymmetrically encrypted text.
    typedef Output Digest;
    /// Represent a cryptographic signature.
    typedef Output Signature;
  }
}

# include <cryptography/rsa/fwd.hh>

#endif
