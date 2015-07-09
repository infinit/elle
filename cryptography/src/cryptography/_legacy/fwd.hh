#ifndef INFINIT_CRYPTOGRAPHY_LEGACY_FWD_HH
# define INFINIT_CRYPTOGRAPHY_LEGACY_FWD_HH

namespace infinit
{
  namespace cryptography
  {
    class Input;
    class Output;
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

#endif
