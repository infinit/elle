#ifndef ELLE_CRYPTOGRAPHY_RSA_SERIALIZATION_HH
# define ELLE_CRYPTOGRAPHY_RSA_SERIALIZATION_HH

# include <openssl/rsa.h>

# include <cryptography/serialization.hh>

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
          typedef ::RSA Type;
          static constexpr const char* identifier = "rsa";
        };
      }
    }
  }
}

#endif
