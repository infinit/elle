#pragma once

#include <elle/cryptography/fwd.hh>
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
          using Type = ::RSA*;
          static constexpr const char* identifier = "rsa";
        };
      }
    }
  }
}
