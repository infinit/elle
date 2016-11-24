#ifndef INFINIT_CRYPTOGRAPHY_DSA_SERIALIZATION_HH
# define INFINIT_CRYPTOGRAPHY_DSA_SERIALIZATION_HH

# include <openssl/dsa.h>

# include <cryptography/serialization.hh>

namespace infinit
{
  namespace cryptography
  {
    namespace dsa
    {
      namespace serialization
      {
        /*-----------.
        | Structures |
        `-----------*/

        struct DSA
        {
          typedef ::DSA Type;
          static constexpr const char* identifier = "dsa";
        };
      }
    }
  }
}

#endif
