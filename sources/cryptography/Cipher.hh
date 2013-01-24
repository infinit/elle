#ifndef INFINIT_CRYPTOGRAPHY_CIPHER_HH
# define INFINIT_CRYPTOGRAPHY_CIPHER_HH

# include <cryptography/Output.hh>

namespace infinit
{
  namespace cryptography
  {
    /// Represent a symetrically ciphered text output from the
    /// SecretKey methods.
    typedef Output Cipher;
  }
}

#endif
