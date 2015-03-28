#ifndef INFINIT_CRYPTOGRAPHY_CIPHER_HH
# define INFINIT_CRYPTOGRAPHY_CIPHER_HH

# include <cryptography/Output.hh>

# include <openssl/evp.h>

namespace infinit
{
  namespace cryptography
  {
    /*-------------.
    | Enumerations |
    `-------------*/

    /// Define the cipher algorithm.
    enum class Cipher
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
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Cipher const cipher);

    namespace cipher
    {
      /*----------.
      | Functions |
      `----------*/

      /// Return the cipher function associated with the given algorithm name.
      ::EVP_CIPHER const*
      resolve(Cipher const name);
    }
  }
}

#endif
