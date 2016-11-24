#ifndef INFINIT_CRYPTOGRAPHY_RSA_SERIALIZATION_HH
# define INFINIT_CRYPTOGRAPHY_RSA_SERIALIZATION_HH

# include <openssl/rsa.h>

# include <cryptography/serialization.hh>

namespace infinit
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
