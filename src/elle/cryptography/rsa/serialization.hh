#pragma once

#include <openssl/rsa.h>

#include <elle/cryptography/serialization.hh>

namespace elle
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace serialization
      {
        /*-----------.
        | Structures |
        `-----------*/

        struct RSA
        {
          using Type = ::RSA;
          static constexpr const char* identifier = "rsa";
        };
      }
    }
  }
}
