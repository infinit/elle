#include <cryptography/deleter.hh>

namespace elle
{
  namespace cryptography
  {
    namespace deleter
    {
      /*-------.
      | BIGNUM |
      `-------*/

      void
      BIGNUM::operator ()(::BIGNUM* bn)
      {
        if (bn != nullptr)
          ::BN_clear_free(bn);
      }

      /*---------.
      | EVP_PKEY |
      `---------*/

      void
      EVP_PKEY::operator ()(::EVP_PKEY* pkey)
      {
        if (pkey != nullptr)
          ::EVP_PKEY_free(pkey);
      }

      /*-------------.
      | EVP_PKEY_CTX |
      `-------------*/

      void
      EVP_PKEY_CTX::operator ()(::EVP_PKEY_CTX* ctx)
      {
        if (ctx != nullptr)
          ::EVP_PKEY_CTX_free(ctx);
      }
    }
  }
}
