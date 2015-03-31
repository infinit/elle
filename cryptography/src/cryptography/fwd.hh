#ifndef INFINIT_CRYPTOGRAPHY_FWD_HH
# define INFINIT_CRYPTOGRAPHY_FWD_HH

namespace infinit
{
  namespace cryptography
  {
    enum class Cipher;
    enum class Mode;
    enum class Cryptosystem;
    enum class Oneway;
    class Exception;
    class Input;
    class Output;
    class SecretKey;
    /// Represent a plain text for cryptographic operations.
    typedef Input Plain;
    /// Represent a decrypted text, both trought symmetric and asymmetric
    /// cryptosystems.
    typedef Output Clear;
    /// Represent an encrypted text, asymmetrically or symmetrically.
    typedef Output Code;
    /// Represent a hashed content.
    typedef Output Digest;
    /// Represent a cryptographic signature.
    typedef Output Signature;
  }
}

# include <cryptography/rsa/fwd.hh>

#endif
