#ifndef INFINIT_CRYPTOGRAPHY_RSA_PADDING_HH
# define INFINIT_CRYPTOGRAPHY_RSA_PADDING_HH

#include <ostream>
#include <openssl/evp.h>

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace padding
      {
        /// Print information regarding the given EVP context's padding.
        void
        print(std::ostream& stream,
              ::EVP_PKEY_CTX* context);
      }
    }
  }
}

#endif
