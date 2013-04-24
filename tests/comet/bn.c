#include "comet.h"

#include <comet/bn.h>
#include <comet/rand.h>

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

    BN_free(n3);
    BN_free(n2);
    BN_free(n1);
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

      DOPENSSL_CHECK(BN_cmp(n1, n2) != 0);
      DOPENSSL_CHECK(BN_cmp(n3, n4) != 0);
      DOPENSSL_CHECK(BN_cmp(n1, n3) == 0);
      DOPENSSL_CHECK(BN_cmp(n2, n4) == 0);

      char* _n1 = BN_bn2hex(n1);
      DOPENSSL_CHECK(_n1 != NULL);
      char* _n2 = BN_bn2hex(n2);
      DOPENSSL_CHECK(_n2 != NULL);

      DOPENSSL_CHECK(strcmp(_n1, "F25A0F836DC27E7FD3CDD7AFE2A14C84D1B12084DB2C14171E7CD525D1DC8B9F58D1F306296799F20DBF0D0629908A5B8098B32077333844BAD291DFCA7D1683F4B17AE2B8C6CBA1172F013101AB0617941E96E7DF3F97A20935569E3172DB9C0520FA6AB698B86E6135C1BA76599F7768300298FEEEDFEFFA39D4EC0812B569") == 0);
      DOPENSSL_CHECK(strcmp(_n2, "ED1B25ABA617994BB998FF236FFF3D60ED5EE4CEC8A7F77CB2161DC395BAD7536077F07FA91DBD78AA5DE6ADD0DF4EFD66324A232362BB9BF0DAA86B93F1D1681D4A56D9E2C0B9F32FA1B65FA16A30A685677AE97DB2C5D8D107484ED4EFB8389BE23CD0ED3BBA79758D9A4D7690F4A910384FB1DBCEA222C8E8497F57E791FCA2A24D27414EF507C5A9CF852D446EDFF46E26868F19B4906092E629BFADC184563E380BECFED675287AFD75B01647654AF57B189A875835205F1F90400EE2408A76EF7A4D743788923C61536C10357508360676D150FD72F8720549786D3504ABEEFFCE70F23DBD7E298127B375B22C3719EFBCF81651249E366A012104C70D") == 0);

      OPENSSL_free(_n2);
      OPENSSL_free(_n1);

      free(fingerprint4);
      free(fingerprint3);
      free(fingerprint2);
      free(fingerprint1);

      BN_free(n4);
      BN_free(n3);
      BN_free(n2);
      BN_free(n1);
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

      DOPENSSL_CHECK(BN_cmp(n1, n2) != 0);
      DOPENSSL_CHECK(BN_cmp(n1, n3) == 0);
      DOPENSSL_CHECK(BN_cmp(n2, n3) != 0);

      free(fingerprint3);
      free(fingerprint2);
      free(fingerprint1);

      BN_free(n3);
      BN_free(n2);
      BN_free(n1);
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
