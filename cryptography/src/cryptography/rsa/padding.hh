#ifndef INFINIT_CRYPTOGRAPHY_RSA_PADDING_HH
# define INFINIT_CRYPTOGRAPHY_RSA_PADDING_HH

# include <elle/types.hh>

# include <openssl/evp.h>

# include <iosfwd>

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
        /// Return the padding footprint --- i.e the number of bits (not bytes)
        /// of data reserved for the padding --- for the given encryption
        /// context.
        elle::Natural32
        footprint(::EVP_PKEY_CTX* context);
      }
    }
  }
}

#endif
