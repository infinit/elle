/*
 * Copyright (c) 2013, infinit.io
 *
 * This software is provided "as is" without warranty of any kind,
 * either expressed or implied, including but not limited to the
 * implied warranties of fitness for a particular purpose.
 *
 * See the LICENSE file for more information on the terms and
 * conditions.
 */

#ifndef DOPENSSL_BN_H
# define DOPENSSL_BN_H

# include <openssl/bn.h>

/* These functions have been slightly modified so as to make sure
   no entropy is generated whenever called. */

int dBN_rand(BIGNUM *rnd, int bits, int top, int bottom);
int dBN_pseudo_rand(BIGNUM *rnd, int bits, int top, int bottom);
int dBN_pseudo_rand_range(BIGNUM *r, const BIGNUM *range);
int dBN_is_prime_fasttest_ex(const BIGNUM *a, int checks, BN_CTX *ctx_passed,
                             int do_trial_division, BN_GENCB *cb);
int dBN_generate_prime_ex(BIGNUM *ret, int bits, int safe,
                          const BIGNUM *add, const BIGNUM *rem, BN_GENCB *cb);

/* Display the given big number on the standard output. */
int dBN_print(BIGNUM *n);

#endif
