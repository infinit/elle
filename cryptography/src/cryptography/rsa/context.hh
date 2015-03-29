#ifndef INFINIT_CRYPTOGRAPHY_RSA_CONTEXT_HH
# define INFINIT_CRYPTOGRAPHY_RSA_CONTEXT_HH

# include <cryptography/rsa/Padding.hh>

# include <openssl/evp.h>

# include <iosfwd>

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace context
      {
        /// Create a cryptographic context.
        ::EVP_PKEY_CTX*
        create(::EVP_PKEY* key,
               int (*function)(EVP_PKEY_CTX*),
               Padding const padding);
      }
    }
  }
}

#endif
