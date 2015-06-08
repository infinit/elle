#ifndef INFINIT_CRYPTOGRAPHY_CONTEXT_HH
# define INFINIT_CRYPTOGRAPHY_CONTEXT_HH

# include <openssl/evp.h>

# include <iosfwd>

namespace infinit
{
  namespace cryptography
  {
    namespace context
    {
      /// Create a cryptographic context.
      ::EVP_PKEY_CTX*
      create(::EVP_PKEY* key,
             int (*function)(EVP_PKEY_CTX*));
    }
  }
}

#endif
