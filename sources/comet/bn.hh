#ifndef COMET_BN_HH
# define COMET_BN_HH

# include <openssl/bn.h>

int dBN_rand(BIGNUM *rnd, int bits, int top, int bottom);
int dBN_pseudo_rand(BIGNUM *rnd, int bits, int top, int bottom);
int dBN_pseudo_rand_range(BIGNUM *r, const BIGNUM *range);
int dBN_is_prime_fasttest_ex(const BIGNUM *a, int checks, BN_CTX *ctx_passed,
                             int do_trial_division, BN_GENCB *cb);
int dBN_generate_prime_ex(BIGNUM *ret, int bits, int safe,
                          const BIGNUM *add, const BIGNUM *rem, BN_GENCB *cb);

#endif
