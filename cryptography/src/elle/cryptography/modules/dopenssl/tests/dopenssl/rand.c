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

#include "dopenssl.h"

#include <dopenssl/bn.h>
#include <dopenssl/rand.h>

#include <openssl/bn.h>
#include <openssl/rand.h>

#include <stdlib.h>
#include <string.h>

/*------.
| Basic |
`------*/

static
void
test_basic()
{
  DOPENSSL_CHECK(dRAND_init() == 1);

  // Note that the result of the RAND_status is not checked because
  // the PNRG has not been seeded, hence does not have enough entropy.

  DOPENSSL_CHECK(dRAND_start() == 1);
  {
    DOPENSSL_CHECK(RAND_get_rand_method() == &dRAND_method);

    RAND_status();
    RAND_status();

    dRAND_reset();

    RAND_status();
  }
  DOPENSSL_CHECK(dRAND_stop() == 1);

  RAND_status();

  DOPENSSL_CHECK(dRAND_clean() == 1);
}

/*-----.
| Seed |
`-----*/

static
void
test_seed()
{
  char const* seed =
    "Sir, an equation has no meaning for me "
    "unless it expresses a thought of GOD.";

  DOPENSSL_CHECK(dRAND_init() == 1);

  DOPENSSL_CHECK(dRAND_start() == 1);
  {
    DOPENSSL_CHECK(RAND_get_rand_method() == &dRAND_method);

    // Reset the random implementation and seed the random generator.
    dRAND_reset();
    RAND_seed(seed, strlen(seed));
    DOPENSSL_CHECK(RAND_status() == 1);
    char* fingerprint1 = dRAND_fingerprint();
    DOPENSSL_CHECK(fingerprint1 != NULL);

    // Re-reset the random implementation and finally re-seed it:
    // the result should be the same as the first seeding.
    dRAND_reset();
    RAND_seed(seed, strlen(seed));
    DOPENSSL_CHECK(RAND_status() == 1);
    char* fingerprint2 = dRAND_fingerprint();
    DOPENSSL_CHECK(fingerprint2 != NULL);

    DOPENSSL_CHECK(strcmp(fingerprint1, fingerprint2) == 0);

    DOPENSSL_CHECK(strcmp(fingerprint1,
                          "a15e5e1860c9297f7055edb960067254822123ab") == 0);

    free(fingerprint2);
    free(fingerprint1);
  }
  DOPENSSL_CHECK(dRAND_stop() == 1);

  DOPENSSL_CHECK(dRAND_clean() == 1);
}

/*-----.
| Main |
`-----*/

int
main(int argc,
     char** argv)
{
  test_basic();
  test_seed();

  return (0);
}
