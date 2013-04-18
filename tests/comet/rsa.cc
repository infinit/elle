#include "comet.hh"

#include <comet/rand.hh>
#include <comet/rsa.hh>

#include <openssl/evp.h>

/*-------------------.
| RSA Key Generation |
`-------------------*/

static
::RSA*
test_generate(unsigned int const length)
{
  ::EVP_PKEY_CTX* context = ::EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
  BOOST_CHECK(context != nullptr);

  BOOST_CHECK(::EVP_PKEY_keygen_init(context) > 0);

  BOOST_CHECK(::EVP_PKEY_CTX_set_rsa_keygen_bits(context, length) > 0);

  ::EVP_PKEY* key = nullptr;

  BOOST_CHECK(::EVP_PKEY_keygen(context, &key) > 0);

  ::RSA* rsa = key->pkey.rsa;
  key->pkey.rsa = nullptr;

  ::EVP_PKEY_free(key);
  ::EVP_PKEY_CTX_free(context);

  return (rsa);
}

/*-------.
| Rotate |
`-------*/

static
void
test_rotate()
{
  const char* seed =
    "Sir, an equation has no meaning for me "
    "unless it expresses a thought of GOD.";
  ::RSA* original = nullptr;
  ::RSA* rotated1 = nullptr;
  ::RSA* rotated2 = nullptr;
  ::RSA* rotated3 = nullptr;
  ::RSA* rotated4 = nullptr;

  BOOST_CHECK(::dRAND_init() == 1);

  // Original key.
  original = test_generate(1024);
  BOOST_CHECK(original != nullptr);

  // Private Deducing 1.
  {
    rotated1 = ::RSA_new();
    BOOST_CHECK(rotated1 != nullptr);
    BOOST_CHECK(::dRSA_deduce_privatekey(rotated1,
                                         ::BN_num_bits(original->n),
                                         (const unsigned char*)seed,
                                         ::strlen(seed)) > 0);
  }

  // PrivateKey Deducing 2.
  {
    rotated2 = ::RSA_new();
    BOOST_CHECK(rotated2 != nullptr);
    BOOST_CHECK(::dRSA_deduce_privatekey(rotated2,
                                         ::BN_num_bits(original->n),
                                         (const unsigned char*)seed,
                                         ::strlen(seed)) > 0);
  }

  // PublicKey Deducing 3.
  {
    rotated3 = ::RSA_new();
    BOOST_CHECK(rotated3 != nullptr);
    BOOST_CHECK(::dRSA_deduce_publickey(rotated3,
                                        rotated1->n,
                                        (const unsigned char*)seed,
                                        ::strlen(seed)) > 0);
  }

  // PublicKey Deducing 4.
  {
    rotated4 = ::RSA_new();
    BOOST_CHECK(rotated4 != nullptr);
    BOOST_CHECK(::dRSA_deduce_publickey(rotated4,
                                        rotated1->n,
                                        (const unsigned char*)seed,
                                        ::strlen(seed)) > 0);
  }

  BOOST_CHECK(::dRSA_cmp_privatekey(rotated1, rotated2) == 0);
  BOOST_CHECK(::dRSA_cmp_publickey(rotated1, rotated3) == 0);
  BOOST_CHECK(::dRSA_cmp_publickey(rotated1, rotated4) == 0);
  BOOST_CHECK(::dRSA_cmp_publickey(rotated2, rotated3) == 0);
  BOOST_CHECK(::dRSA_cmp_publickey(rotated2, rotated4) == 0);
  BOOST_CHECK(::dRSA_cmp_publickey(rotated3, rotated4) == 0);

  char* n = ::BN_bn2hex(rotated1->n);
  BOOST_CHECK(n != nullptr);
  char* e = ::BN_bn2hex(rotated1->e);
  BOOST_CHECK(e != nullptr);
  char* d = ::BN_bn2hex(rotated1->d);
  BOOST_CHECK(d != nullptr);
  char* p = ::BN_bn2hex(rotated1->p);
  BOOST_CHECK(p != nullptr);
  char* q = ::BN_bn2hex(rotated1->q);
  BOOST_CHECK(q != nullptr);
  char* dmp1 = ::BN_bn2hex(rotated1->dmp1);
  BOOST_CHECK(dmp1 != nullptr);
  char* dmq1 = ::BN_bn2hex(rotated1->dmq1);
  BOOST_CHECK(dmq1 != nullptr);
  char* iqmp = ::BN_bn2hex(rotated1->iqmp);
  BOOST_CHECK(iqmp != nullptr);

  BOOST_CHECK(::strcmp(n, "C6A46767815C22D0E090F6B77F73BC3FAE11DA9B5E7FD4D237FCB5EDB4196531826B6C845CAB630A497B837BA8709983FAC2957BAE9413A60A2B27848244CF7DF136C9819497B681F9E1A79D45ABF9E39257409AA5CB84E06A6CF22A7856E15FD85952D199730EFB115F61566BEAC78C0B48B2A142E5780E196C1DF1A4D51F9B") == 0);
  BOOST_CHECK(::strcmp(e, "01881691DA7C11AA660FFA84A9A20862B2BDDA3DEB5B0A657413E8B6CE9B257BB2DF021DE847BA735A274BFB080DE0F0CF6066473CCE8BF2B045400C953A66F7DB") == 0);
  BOOST_CHECK(::strcmp(d, "8C12E4BCDA0D27A491FD69BC2DFD2E9952262DD6E1F96ED088E538BDE72AEA70AA0198F9A4BDBB9BA45C47355B59B566F26389DD1F99880659AEC495BBBCACAE805157A6903529BBBBAEE0D4A8C4A9E5D105B5F24976B3DD15F6B8ADEA7FDBF25EF0CDB87C9237E7A0298C325698BC291AB4753CB991477D5F26D8F276DE53D3") == 0);
  BOOST_CHECK(::strcmp(p, "F750F9BA6507C618B7DFF9B86D4FDC53386FFC742B4A446460D2B2410759082E78D134BF70745A1D1F540B847DEC9F057AAA7AFC3058217AAA9E167BEF6483F1") == 0);
  BOOST_CHECK(::strcmp(q, "CD9DEABA657C67667AB87C874C4EC2A71293FFD7BDF0B6B7E350949138DA1CE28356F285A5A543116FA0BF028C2112F31952C3B6D1DDA319E99D1DACA8A1F84B") == 0);
  BOOST_CHECK(::strcmp(dmp1, "412166ECC917213A53BC83FB684CD4CF265306B9CD5EB1E97C9C4B449241EE2E42BA383CD0EA483932F9CE188BF84D700C00F35CC0E9BB9E98C2832531DAFE23") == 0);
  BOOST_CHECK(::strcmp(dmq1, "040A59A9664AE258A9D8E2A1B3875123539D4D7438EAB5E6B61D651194584631AF6CC2814E044B750D9C4E9AA04D4BB69AA5DA601EF4C2917D327480311CE29F") == 0);
  BOOST_CHECK(::strcmp(iqmp, "F0350C41F9599E9EB521B370F50A152AF65FDC3DE54A7C0322C439FDC2C8A8C6B6FF4BD2220A213F588CCFA6649D920404498A7A1037D7B6E5AAC24696B81918") == 0);

  ::OPENSSL_free(iqmp);
  ::OPENSSL_free(dmq1);
  ::OPENSSL_free(dmp1);
  ::OPENSSL_free(q);
  ::OPENSSL_free(p);
  ::OPENSSL_free(d);
  ::OPENSSL_free(e);
  ::OPENSSL_free(n);

  ::RSA_free(rotated4);
  ::RSA_free(rotated3);
  ::RSA_free(rotated2);
  ::RSA_free(rotated1);
  ::RSA_free(original);

  BOOST_CHECK(::dRAND_clean() == 1);
}

/*-----.
| Main |
`-----*/

static
bool
test()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("rsa");

  suite->add(BOOST_TEST_CASE(test_rotate));

  boost::unit_test::framework::master_test_suite().add(suite);

  return (true);
}

int
main(int argc,
     char** argv)
{
  return (boost::unit_test::unit_test_main(test, argc, argv));
}
