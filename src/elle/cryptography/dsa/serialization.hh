#ifndef ELLE_CRYPTOGRAPHY_DSA_SERIALIZATION_HH
# define ELLE_CRYPTOGRAPHY_DSA_SERIALIZATION_HH

# include <openssl/dsa.h>

# include <elle/cryptography/serialization.hh>

namespace elle
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
