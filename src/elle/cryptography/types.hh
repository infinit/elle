#pragma once

#include <memory>

#include <elle/cryptography/deleter.hh>

#include <openssl/bn.h>
#include <openssl/evp.h>

namespace elle
{
  namespace cryptography
  {
    /// Define some smart pointer types for low-level structures.
    namespace types
    {
      using BIGNUM = std::unique_ptr<BIGNUM, deleter::BIGNUM>;
      using EVP_PKEY = std::unique_ptr<EVP_PKEY, deleter::EVP_PKEY>;
      using EVP_PKEY_CTX = std::unique_ptr<EVP_PKEY_CTX, deleter::EVP_PKEY_CTX>;
    }
  }
}
