#ifndef INFINIT_CRYPTOGRAPHY_EVP_HH
# define INFINIT_CRYPTOGRAPHY_EVP_HH

# include <cryptography/fwd.hh>
# include <cryptography/Code.hh>

# include <openssl/evp.h>

namespace infinit
{
  namespace cryptography
  {
    namespace evp
    {
      /*----------.
      | Functions |
      `----------*/

      /// Encrypt the given plain with the provided encryption context and
      /// function.
      Code
      encrypt(Plain const& plain,
              ::EVP_PKEY_CTX* context,
              int (*function)(EVP_PKEY_CTX*,
                              unsigned char*,
                              size_t*,
                              const unsigned char*,
                              size_t));
      /// Decrypt the code with the provided context and function.
      Clear
      decrypt(Code const& code,
              ::EVP_PKEY_CTX* context,
              int (*function)(EVP_PKEY_CTX*,
                              unsigned char*,
                              size_t*,
                              const unsigned char*,
                              size_t));
      /// Return true if the signature is valid according to the given plain.
      elle::Boolean
      verify(Signature const& signature,
             Plain const& plain,
             ::EVP_PKEY_CTX* context,
             int (*function)(EVP_PKEY_CTX*,
                             const unsigned char*,
                             size_t,
                             const unsigned char*,
                             size_t));
      /// Sign the given plain text.
      Signature
      sign(Plain const& plain,
           ::EVP_PKEY_CTX* context,
           int (*function)(EVP_PKEY_CTX*,
                           unsigned char*,
                           size_t*,
                           const unsigned char*,
                           size_t));
    }
  }
}

#endif
