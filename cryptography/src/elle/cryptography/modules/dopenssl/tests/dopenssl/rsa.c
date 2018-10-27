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

#include <dopenssl/rand.h>
#include <dopenssl/rsa.h>

#include <openssl/evp.h>

#include <stdlib.h>
#include <string.h>

/*-------------------.
| RSA Key Generation |
`-------------------*/

static
EVP_PKEY*
_test_generate(unsigned int const length)
{
  EVP_PKEY_CTX* context = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
  DOPENSSL_CHECK(context != NULL);

  DOPENSSL_CHECK(EVP_PKEY_keygen_init(context) > 0);

  DOPENSSL_CHECK(EVP_PKEY_CTX_set_rsa_keygen_bits(context, length) > 0);

  EVP_PKEY* key = NULL;

  DOPENSSL_CHECK(EVP_PKEY_keygen(context, &key) > 0);

  EVP_PKEY_CTX_free(context);

  return (key);
}

/*---------------.
| RSA Encryption |
`---------------*/

static
void
_test_encrypt(EVP_PKEY* key,
              unsigned char const* clear,
              size_t const clear_size,
              unsigned char** code,
              size_t* code_size)
{
  // Prepare the context.

  EVP_PKEY_CTX* context = EVP_PKEY_CTX_new(key, NULL);
  DOPENSSL_CHECK(context != NULL);

  DOPENSSL_CHECK(EVP_PKEY_sign_init(context) > 0);

  DOPENSSL_CHECK(EVP_PKEY_CTX_ctrl(context,
                                  EVP_PKEY_RSA,
                                  -1,
                                  EVP_PKEY_CTRL_RSA_PADDING,
                                  RSA_NO_PADDING, // WARNING: insecure by design
                                  NULL) > 0);

  // Encrypt with the private key.

  size_t size;

  DOPENSSL_CHECK(clear != NULL);
  DOPENSSL_CHECK(clear_size > 0);

  DOPENSSL_CHECK(EVP_PKEY_sign(context,
                              NULL,
                              &size,
                              clear,
                              clear_size) > 0);

  DOPENSSL_CHECK(code != NULL);
  *code = (unsigned char*)OPENSSL_malloc(size);

  DOPENSSL_CHECK(EVP_PKEY_sign(context,
                              *code,
                              &size,
                              clear,
                              clear_size) > 0);

  DOPENSSL_CHECK(code_size != NULL);
  *code_size = size;

  // Clean the resources.

  EVP_PKEY_CTX_free(context);
}

/*---------------.
| RSA Decryption |
`---------------*/

static
void
_test_decrypt(EVP_PKEY* key,
              unsigned char const* code,
              size_t const code_size,
              unsigned char** clear,
              size_t* clear_size)
{
  // Prepare the context.

  EVP_PKEY_CTX *context = EVP_PKEY_CTX_new(key, NULL);
  DOPENSSL_CHECK(context != NULL);

  DOPENSSL_CHECK(EVP_PKEY_verify_recover_init(context) > 0);

  DOPENSSL_CHECK(EVP_PKEY_CTX_ctrl(context,
                                  EVP_PKEY_RSA,
                                  -1,
                                  EVP_PKEY_CTRL_RSA_PADDING,
                                  RSA_NO_PADDING, // WARNING: insecure by design
                                  NULL) > 0);

  // Decrypt with the public key.

  size_t size;

  DOPENSSL_CHECK(code != NULL);
  DOPENSSL_CHECK(code_size > 0);

  DOPENSSL_CHECK(EVP_PKEY_verify_recover(context,
                                        NULL,
                                        &size,
                                        code,
                                        code_size) > 0);

  DOPENSSL_CHECK(clear != NULL);
  *clear = (unsigned char*)OPENSSL_malloc(size);

  DOPENSSL_CHECK(EVP_PKEY_verify_recover(context,
                                        *clear,
                                        &size,
                                        code,
                                        code_size) > 0);

  DOPENSSL_CHECK(clear != NULL);
  *clear_size = size;

  // Clean the resources.

  EVP_PKEY_CTX_free(context);
}

/*-------.
| Deduce |
`-------*/

static
void
test_deduce()
{
  char const* seed =
    "Sir, an equation has no meaning for me "
    "unless it expresses a thought of GOD.";

  DOPENSSL_CHECK(dRAND_init() == 1);

  // Original key.
  EVP_PKEY* key = _test_generate(1024);
  DOPENSSL_CHECK(key != NULL);

  RSA* original = key->pkey.rsa;
  RSA* deduced1 = NULL;
  RSA* deduced2 = NULL;
  RSA* deduced3 = NULL;
  RSA* deduced4 = NULL;

  // Private Deducing 1.
  {
    deduced1 = dRSA_deduce_privatekey(BN_num_bits(original->n),
                                      (unsigned char const*)seed,
                                      strlen(seed));
    DOPENSSL_CHECK(deduced1 != NULL);
  }

  // PrivateKey Deducing 2.
  {
    deduced2 = dRSA_deduce_privatekey(BN_num_bits(original->n),
                                      (unsigned char const*)seed,
                                      strlen(seed));
    DOPENSSL_CHECK(deduced2 != NULL);
  }

  // PublicKey Deducing 3.
  {
    deduced3 = dRSA_deduce_publickey(deduced1->n,
                                     (unsigned char const*)seed,
                                     strlen(seed));
    DOPENSSL_CHECK(deduced3 != NULL);
  }

  // PublicKey Deducing 4.
  {
    deduced4 = dRSA_deduce_publickey(deduced1->n,
                                     (unsigned char const*)seed,
                                     strlen(seed));
    DOPENSSL_CHECK(deduced4 != NULL);
  }

  DOPENSSL_CHECK(dRSA_cmp_privatekey(deduced1, deduced2) == 0);
  DOPENSSL_CHECK(dRSA_cmp_publickey(deduced1, deduced3) == 0);
  DOPENSSL_CHECK(dRSA_cmp_publickey(deduced1, deduced4) == 0);
  DOPENSSL_CHECK(dRSA_cmp_publickey(deduced2, deduced3) == 0);
  DOPENSSL_CHECK(dRSA_cmp_publickey(deduced2, deduced4) == 0);
  DOPENSSL_CHECK(dRSA_cmp_publickey(deduced3, deduced4) == 0);

  char* n = BN_bn2hex(deduced1->n);
  DOPENSSL_CHECK(n != NULL);
  char* e = BN_bn2hex(deduced1->e);
  DOPENSSL_CHECK(e != NULL);
  char* d = BN_bn2hex(deduced1->d);
  DOPENSSL_CHECK(d != NULL);
  char* p = BN_bn2hex(deduced1->p);
  DOPENSSL_CHECK(p != NULL);
  char* q = BN_bn2hex(deduced1->q);
  DOPENSSL_CHECK(q != NULL);
  char* dmp1 = BN_bn2hex(deduced1->dmp1);
  DOPENSSL_CHECK(dmp1 != NULL);
  char* dmq1 = BN_bn2hex(deduced1->dmq1);
  DOPENSSL_CHECK(dmq1 != NULL);
  char* iqmp = BN_bn2hex(deduced1->iqmp);
  DOPENSSL_CHECK(iqmp != NULL);

  DOPENSSL_CHECK(strcmp(n, "D37F7E18E74D26101AA4DB65618C00553D86A25D398D6F112432561EA6BA1DFCA4ECD18AF9958973A56E9EC07222E42F7E84B1625554140AD3E840FFC81BC4BF830D49820A238A2C07846067D796FBD457A6B491B165E022D278D538EA6A197CBE548F6E669E0E7F3F6AA0907D4C5C41BAA80C191297A02A2963C7CC4B877025") == 0);
  DOPENSSL_CHECK(strcmp(e, "0190CB035742DEE8D2053AE254258DDA5422013425BDB1F8F4BA2C12AFCE48E8869273B7153ECAFE1D1C1AA9D38B54CAC2F0E6045C4009008235CBF11471527293") == 0);
  DOPENSSL_CHECK(strcmp(d, "BC412C05E9A4781E55830A43918487AB9765B18276C09DB8B996B281DB19596F064E0BF42F4D5E6329A762283979E06E7790CB1B02BBDE7EE6CE1B595FBA8511AC7D71034C5C0E338F6B90C57AE7E151F129B8D9076B0AFE9D078FEA4B35E2C3B161B20CEED2C5C19B7BADB231F51516404235E8EA4DBF97BEEE9E432D523B9B") == 0);
  DOPENSSL_CHECK(strcmp(p, "F0213E24F84C720370B550AA3A509B24E4D2B9B86482F5FA019E86D1454C67168B9C04DC9DE94E927E4452F23ED54F583E89AB1FED1B92678951483DD0174071") == 0);
  DOPENSSL_CHECK(strcmp(q, "E179D3E6F4AD5BDA9ADCEFA482B9711F94C6DDE17B57EDBCFEDD17BA356A837DDD83BB5FB8131105FDDF40FE7470CD2E7CFE03DFCA3DC9FA26437472EF1504F5") == 0);
  DOPENSSL_CHECK(strcmp(dmp1, "E5776B1025625319B2E1D9AD36B1741CF110D32417253B995AA95BBA4FECB09E88084C0E680CB3829019D3208F78857293F3F6B81C26B6E2B08ADE315B89868B") == 0);
  DOPENSSL_CHECK(strcmp(dmq1, "6B5C5B711D31EF831FB0514BB479D927467B5526E61F4BEB929DBB04A5324830C0F0A220D6CFB32C1E6EA5EAE3E30ACA0B3E9313302598C8DC4B8F30B3A44297") == 0);
  DOPENSSL_CHECK(strcmp(iqmp, "43B816E00C7BFC443ECE271127DDE351E76A4D85C69F78C0DC79E5349FEB2EE56A9B2F3811A66D6F04A85B7DAB983C4C69C6525672FD637207463C5F7FB78392") == 0);

  OPENSSL_free(iqmp);
  OPENSSL_free(dmq1);
  OPENSSL_free(dmp1);
  OPENSSL_free(q);
  OPENSSL_free(p);
  OPENSSL_free(d);
  OPENSSL_free(e);
  OPENSSL_free(n);

  RSA_free(deduced4);
  RSA_free(deduced3);
  RSA_free(deduced2);
  RSA_free(deduced1);

  EVP_PKEY_free(key);

  DOPENSSL_CHECK(dRAND_clean() == 1);
}

/*----------------.
| Rotate & Derive |
`----------------*/

static
void
test_rotate_and_derive()
{
  // Initializing.
  DOPENSSL_CHECK(dRAND_init() == 1);

  // Note that the seed below has been carefully selected for its length to
  // match the RSA key size.
  char const* seed =
    "Before creation God did just pure mathematics. Then He thought it would "
    "be a pleasant change to do some applied. ---- Littlewood";

  EVP_PKEY* key = _test_generate(1024);

  BIGNUM* N = BN_dup(key->pkey.rsa->n);

  // Rotation 1.
  unsigned char* rotated1_seed = NULL;
  size_t rotated1_seed_size = 0;
  RSA* rotated1_rsa;

  _test_encrypt(key,
                (unsigned char const*)seed, strlen(seed),
                &rotated1_seed, &rotated1_seed_size);

  rotated1_rsa = dRSA_deduce_privatekey(BN_num_bits(N),
                                        (unsigned char const*)rotated1_seed,
                                        rotated1_seed_size);
  DOPENSSL_CHECK(rotated1_rsa != NULL);

  // Rotation 2.
  unsigned char* rotated2_seed = NULL;
  size_t rotated2_seed_size = 0;
  RSA* rotated2_rsa;

  _test_encrypt(key,
                rotated1_seed, rotated1_seed_size,
                &rotated2_seed, &rotated2_seed_size);

  rotated2_rsa = dRSA_deduce_privatekey(BN_num_bits(N),
                                          (unsigned char const*)rotated2_seed,
                                          rotated2_seed_size);
  DOPENSSL_CHECK(rotated2_rsa != NULL);

  // Rotation 3.
  unsigned char* rotated3_seed = NULL;
  size_t rotated3_seed_size = 0;
  RSA* rotated3_rsa;

  _test_encrypt(key,
                rotated2_seed, rotated2_seed_size,
                &rotated3_seed, &rotated3_seed_size);

  rotated3_rsa = dRSA_deduce_privatekey(BN_num_bits(N),
                                          (unsigned char const*)rotated3_seed,
                                          rotated3_seed_size);
  DOPENSSL_CHECK(rotated3_rsa != NULL);

  // Derivation 1.
  unsigned char* derived1_seed = NULL;
  size_t derived1_seed_size = 0;
  RSA* derived1_rsa;

  _test_decrypt(key,
                rotated3_seed, rotated3_seed_size,
                &derived1_seed, &derived1_seed_size);

  derived1_rsa = dRSA_deduce_privatekey(BN_num_bits(N),
                                          (unsigned char const*)derived1_seed,
                                          derived1_seed_size);
  DOPENSSL_CHECK(derived1_rsa != NULL);

  // Derivation 2.
  unsigned char* derived2_seed = NULL;
  size_t derived2_seed_size = 0;
  RSA* derived2_rsa;

  _test_decrypt(key,
                derived1_seed, derived1_seed_size,
                &derived2_seed, &derived2_seed_size);

  derived2_rsa = dRSA_deduce_privatekey(BN_num_bits(N),
                                          (unsigned char const*)derived2_seed,
                                          derived2_seed_size);
  DOPENSSL_CHECK(derived2_rsa != NULL);

  // Comparisons of the following states.
  //
  //   seed
  //       '
  //        '
  //        rotated1_seed
  //            '
  //             '
  //             rotated2_seed
  //                 '
  //                  '
  //                  rotated3_seed
  //                  '
  //                 '
  //             derived1_seed
  //             '
  //            '
  //        derived2_seed
  DOPENSSL_CHECK(rotated2_seed_size == derived1_seed_size);
  DOPENSSL_CHECK(memcmp(rotated2_seed, derived1_seed, rotated2_seed_size) == 0);
  DOPENSSL_CHECK(rotated1_seed_size == derived2_seed_size);
  DOPENSSL_CHECK(memcmp(rotated1_seed, derived2_seed, rotated1_seed_size) == 0);

  DOPENSSL_CHECK(dRSA_cmp_privatekey(rotated2_rsa, derived1_rsa) == 0);
  DOPENSSL_CHECK(dRSA_cmp_privatekey(rotated1_rsa, derived2_rsa) == 0);

  // Cleaning.

  RSA_free(derived2_rsa);
  OPENSSL_free(derived2_seed);
  RSA_free(derived1_rsa);
  OPENSSL_free(derived1_seed);
  RSA_free(rotated3_rsa);
  OPENSSL_free(rotated3_seed);
  RSA_free(rotated2_rsa);
  OPENSSL_free(rotated2_seed);
  RSA_free(rotated1_rsa);
  OPENSSL_free(rotated1_seed);

  BN_clear_free(N);

  EVP_PKEY_free(key);

  DOPENSSL_CHECK(dRAND_clean() == 1);
}

/*-----.
| Main |
`-----*/

int
main(int argc,
     char** argv)
{
  test_deduce();
  test_rotate_and_derive();

  return (0);
}
