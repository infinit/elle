#ifndef ELLE_CRYPTOGRAPHY_CONTEXT_HH
# define ELLE_CRYPTOGRAPHY_CONTEXT_HH

# include <iosfwd>
# include <elle/cryptography/fwd.hh>

namespace elle
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
