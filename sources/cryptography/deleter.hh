#ifndef INFINIT_CRYPTOGRAPHY_DELETER_HH
# define INFINIT_CRYPTOGRAPHY_DELETER_HH

# include <openssl/bn.h>
# include <openssl/evp.h>

/// These deleters can be used with the types defined in types.hh so as
/// to use smart pointers with such low-level structures.

namespace infinit
{
  namespace cryptography
  {
    namespace deleter
    {
      /*-------.
      | BIGNUM |
      `-------*/

      struct BIGNUM
      {
        void
        operator ()(::BIGNUM* bn);
      };

      /*---------.
      | EVP_PKEY |
      `---------*/

      struct EVP_PKEY
      {
        void
        operator ()(::EVP_PKEY* pkey);
      };

      /*-------------.
      | EVP_PKEY_CTX |
      `-------------*/

      struct EVP_PKEY_CTX
      {
        void
        operator ()(::EVP_PKEY_CTX* ctx);
      };
    }
  }
}

#endif
