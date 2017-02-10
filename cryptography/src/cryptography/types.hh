#ifndef ELLE_CRYPTOGRAPHY_TYPES_HH
# define ELLE_CRYPTOGRAPHY_TYPES_HH

# include <memory>

# include <cryptography/deleter.hh>

# include <openssl/bn.h>
# include <openssl/evp.h>

namespace elle
{
  namespace cryptography
  {
    /// Define some smart pointer types for low-level structures.
    namespace types
    {
      typedef std::unique_ptr<BIGNUM,
                              deleter::BIGNUM> BIGNUM;
      typedef std::unique_ptr<EVP_PKEY,
                              deleter::EVP_PKEY> EVP_PKEY;
      typedef std::unique_ptr<EVP_PKEY_CTX,
                              deleter::EVP_PKEY_CTX> EVP_PKEY_CTX;
    }
  }
}

#endif
