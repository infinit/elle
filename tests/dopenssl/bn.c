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

/*---------------.
| Generate Prime |
`---------------*/

static
void
test_generate_prime()
{
  DOPENSSL_CHECK(dRAND_init() == 1);

  // Undeterministically randomly generate numbers should should therefore
  // all be different, with high probability.
  {
    BIGNUM* n1 = BN_new();
    DOPENSSL_CHECK(BN_generate_prime_ex(n1, 1024, 0,
                                        NULL, NULL, NULL) == 1);

    BIGNUM* n2 = BN_new();
    DOPENSSL_CHECK(BN_generate_prime_ex(n2, 1024, 0,
                                        NULL, NULL, NULL) == 1);

    BIGNUM* n3 = BN_new();
    DOPENSSL_CHECK(BN_generate_prime_ex(n3, 1024, 0,
                                        NULL, NULL, NULL) == 1);

    DOPENSSL_CHECK(BN_cmp(n1, n2) != 0);
    DOPENSSL_CHECK(BN_cmp(n1, n3) != 0);
    DOPENSSL_CHECK(BN_cmp(n2, n3) != 0);

    BN_clear_free(n3);
    BN_clear_free(n2);
    BN_clear_free(n1);
  }

  // Generate numbers in a deterministic (but probabilist) way, by resetting the
  // seed for every random generation.
  //
  // Since based on the same seed, all the generated numbers should be equal.
  {
    char const* seed =
      "God exists since mathematics is consistent, "
      "and the Devil exists since we cannot prove it.";

    DOPENSSL_CHECK(dRAND_start() == 1);
    {
      DOPENSSL_CHECK(RAND_get_rand_method() == &dRAND_method);

      dRAND_reset();
      RAND_seed(seed, strlen(seed));
      BIGNUM* n1 = BN_new();
      char* fingerprint1 = dRAND_fingerprint();
      DOPENSSL_CHECK(fingerprint1 != NULL);
      DOPENSSL_CHECK(RAND_status() == 1);
      DOPENSSL_CHECK(dBN_generate_prime_ex(n1, 1024, 0,
                                           NULL, NULL, NULL) == 1);
      BIGNUM* n2 = BN_new();
      char* fingerprint2 = dRAND_fingerprint();
      DOPENSSL_CHECK(fingerprint2 != NULL);
      DOPENSSL_CHECK(RAND_status() == 1);
      DOPENSSL_CHECK(dBN_generate_prime_ex(n2, 2048, 0,
                                           NULL, NULL, NULL) == 1);

      dRAND_reset();
      RAND_seed(seed, strlen(seed));
      BIGNUM* n3 = BN_new();
      char* fingerprint3 = dRAND_fingerprint();
      DOPENSSL_CHECK(fingerprint3 != NULL);
      DOPENSSL_CHECK(RAND_status() == 1);
      DOPENSSL_CHECK(dBN_generate_prime_ex(n3, 1024, 0,
                                           NULL, NULL, NULL) == 1);
      BIGNUM* n4 = BN_new();
      char* fingerprint4 = dRAND_fingerprint();
      DOPENSSL_CHECK(fingerprint2 != NULL);
      DOPENSSL_CHECK(RAND_status() == 1);
      DOPENSSL_CHECK(dBN_generate_prime_ex(n4, 2048, 0,
                                           NULL, NULL, NULL) == 1);

      DOPENSSL_CHECK(strcmp(fingerprint1, fingerprint2) != 0);
      DOPENSSL_CHECK(strcmp(fingerprint3, fingerprint4) != 0);
      DOPENSSL_CHECK(strcmp(fingerprint1, fingerprint3) == 0);
      DOPENSSL_CHECK(strcmp(fingerprint2, fingerprint4) == 0);

      DOPENSSL_CHECK(strcmp(fingerprint1,
                            "7ca2690d012e7b083d2975418e071fbfd20e41aa") == 0);
      DOPENSSL_CHECK(strcmp(fingerprint2,
                            "39c11e1f1ee7c242e26876e3bbb33c2790afd48d") == 0);

      DOPENSSL_CHECK(BN_cmp(n1, n2) != 0);
      DOPENSSL_CHECK(BN_cmp(n3, n4) != 0);
      DOPENSSL_CHECK(BN_cmp(n1, n3) == 0);
      DOPENSSL_CHECK(BN_cmp(n2, n4) == 0);

      char* _n1 = BN_bn2hex(n1);
      DOPENSSL_CHECK(_n1 != NULL);
      char* _n2 = BN_bn2hex(n2);
      DOPENSSL_CHECK(_n2 != NULL);

      DOPENSSL_CHECK(strcmp(_n1, "C74382DE0EBE52A0127E37037452562FA88BAB317D3F096F9FCADFB069A0775B5C28EF7543A4D3A5BAA427D49D63CAD7AEAB5580056ECCC53D0714ACD893C8EAF9B7A094CE0AE3EEE7B5159FB3254061EE09D754D23414659BC91072D884CF6882517B984998CB052C6CF9D8D67C5C974EF475B6BEA45735B381A402FF6137C7") == 0);
      DOPENSSL_CHECK(strcmp(_n2, "DFE1212C3F96ED6AFD51EFDA80882B5DE38FD28F7EF0A9FB7E18C108AB59AFA942CE0C222CF48F0315326478269B3807D0E2D72B09A506C46ABBB8B9FA9258430134E117D711AFE69A0C7FBB5E3DB21F01FBEFCDC36E91EF461DC14490246C63C66FF0FF4BE03009416B85B93DC2083CD267788EDACC59F7CE1DD9D39A3115F1E99C3B2F2774BFCE5A25C593EB04B541A0CBE9DBA3C2A464C89AE590251701C27216E265C0EE6327BE8FB1D3B0C0BD70E3A775F23E23E1091CE37777D5C351DB6EC60E75E020F752CEF08F355117CDB79E4B428F585BFE02358F10971A42789CCD310D71FC7E944B6A53E4133F093C135B3BD9F0C1B15D8D63BA250BE691B51D") == 0);

      OPENSSL_free(_n2);
      OPENSSL_free(_n1);

      free(fingerprint4);
      free(fingerprint3);
      free(fingerprint2);
      free(fingerprint1);

      BN_clear_free(n4);
      BN_clear_free(n3);
      BN_clear_free(n2);
      BN_clear_free(n1);
    }
    DOPENSSL_CHECK(dRAND_stop() == 1);
  }

  // Likewise, generate numbers in a deterministic but safe way.
  {
    char const* seed =
      "A mathematician, like a painter or poet, is a maker of patterns. "
      "If his patterns are more permanent than theirs, it is because they "
      "are made with ideas.";

    DOPENSSL_CHECK(dRAND_start() == 1);
    {
      DOPENSSL_CHECK(RAND_get_rand_method() == &dRAND_method);

      BIGNUM* n1 = BN_new();
      dRAND_reset();
      RAND_seed(seed, strlen(seed));
      char* fingerprint1 = dRAND_fingerprint();
      DOPENSSL_CHECK(fingerprint1 != NULL);
      DOPENSSL_CHECK(RAND_status() == 1);
      DOPENSSL_CHECK(dBN_generate_prime_ex(n1, 512, 1,
                                           NULL, NULL, NULL) == 1);

      BIGNUM* n2 = BN_new();
      dRAND_reset();
      RAND_seed(seed, strlen(seed));
      char* fingerprint2 = dRAND_fingerprint();
      DOPENSSL_CHECK(fingerprint2 != NULL);
      DOPENSSL_CHECK(RAND_status() == 1);
      DOPENSSL_CHECK(dBN_generate_prime_ex(n2, 2048, 1,
                                           NULL, NULL, NULL) == 1);

      BIGNUM* n3 = BN_new();
      dRAND_reset();
      RAND_seed(seed, strlen(seed));
      char* fingerprint3 = dRAND_fingerprint();
      DOPENSSL_CHECK(fingerprint3 != NULL);
      DOPENSSL_CHECK(RAND_status() == 1);
      DOPENSSL_CHECK(dBN_generate_prime_ex(n3, 512, 1,
                                           NULL, NULL, NULL) == 1);

      DOPENSSL_CHECK(strcmp(fingerprint1, fingerprint2) == 0);
      DOPENSSL_CHECK(strcmp(fingerprint1, fingerprint3) == 0);
      DOPENSSL_CHECK(strcmp(fingerprint2, fingerprint3) == 0);

      DOPENSSL_CHECK(strcmp(fingerprint1,
                            "6481dee291c5da35b63dbbd100fd290c32bba44c") == 0);

      DOPENSSL_CHECK(BN_cmp(n1, n2) != 0);
      DOPENSSL_CHECK(BN_cmp(n1, n3) == 0);
      DOPENSSL_CHECK(BN_cmp(n2, n3) != 0);

      char* _n1 = BN_bn2hex(n1);
      DOPENSSL_CHECK(_n1 != NULL);
      char* _n2 = BN_bn2hex(n2);
      DOPENSSL_CHECK(_n2 != NULL);

      DOPENSSL_CHECK(strcmp(_n1, "C62C91629AE1A666FD67619C5654E18F318F1B699763585E70D6355F93FF8ED633AD0E61A10E7F441F4A48961043C35926AE95714D72C113C0E27D21F122E7CF") == 0);
      DOPENSSL_CHECK(strcmp(_n2, "E2B61676A93745EED896C6C84B43BFA1515CDCCA40B55957B8C5470B25B70F80BFD2772A8A3F78ADF72D4CDEE24ABE0244EF3D0C3EAF848004A5D05F0AAFE3E880C93EB97B53D73C76E3D52170E06D32C9B3C7C5ECAF9EEF3EE1044C65723B4F99193B6668C179A8E7547A543D82368B091B28DD8AFDDEA6EBCEB3FA308E11D500A3C7CBF16A7CDB031B81317026852B4A2E24B3B82B58CFAB27A61F5F4CD09327BEBDE12AE5A2A395DA46DCF84B3E488B1E09856A65BC37D615DC43D727900E1D753E22ECF4B174E9BFF00CA6256ACB1F8A5D4DD9FF415A0E67AC4E5A1C2FDC9BFB64E474956E45EDFAAD776A3FC731A5D4E9098E739EB378ECE56469D02253") == 0);

      free(fingerprint3);
      free(fingerprint2);
      free(fingerprint1);

      BN_clear_free(n3);
      BN_clear_free(n2);
      BN_clear_free(n1);
    }
    DOPENSSL_CHECK(dRAND_stop() == 1);
  }

  DOPENSSL_CHECK(dRAND_clean() == 1);
}

/*-----.
| Main |
`-----*/

int
main(int argc,
     char** argv)
{
  test_generate_prime();

  return (0);
}
