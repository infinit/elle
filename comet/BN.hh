#ifndef COMET_BNL_HH
# define COMET_BN_HH

# include <openssl/bn.h>

namespace comet
{

  int BN_generate_prime_ex(BIGNUM *ret, int bits, int safe,
                           const BIGNUM *add, const BIGNUM *rem, BN_GENCB *cb);

}

#endif
