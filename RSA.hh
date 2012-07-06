#ifndef COMET_RSA_HH
# define COMET_RSA_HH

#include <openssl/rsa.h>

namespace comet
{

  int RSA_rotate(RSA *rsa, int bits, const unsigned char *seed, size_t length);
  int RSA_derive(RSA *rsa, BIGNUM* N, const unsigned char *seed, size_t length);

}

#endif
