#ifndef INFINIT_CRYPTOGRAPHY_CIPHER_HH
# define INFINIT_CRYPTOGRAPHY_CIPHER_HH

# include <cryptography/Output.hh>

# include <openssl/evp.h>

namespace infinit
{
  namespace cryptography
  {
    /*------.
    | Types |
    `------*/

    /// Represent a symetrically ciphered text output from the
    /// SecretKey methods.
    typedef Output Cipher;

    /// Contain some constants related to cipher algorithms.
    namespace cipher
    {
      /*-------------.
      | Enumerations |
      `-------------*/

      /// Define the cipher algorithm.
      enum class Algorithm
      {
        des,
        des2,
        des3,
        desx,
        idea,
        rc2,
        blowfish,
        cast5,
        aes128,
        aes192,
        aes256
      };

      /*----------.
      | Functions |
      `----------*/

      /// return the cipher function associated with the given algorithm name.
      ::EVP_CIPHER const*
      resolve(Algorithm const name);

      /*----------.
      | Operators |
      `----------*/

      std::ostream&
      operator <<(std::ostream& stream,
                  Algorithm const algorithm);
    }
  }
}

#endif
