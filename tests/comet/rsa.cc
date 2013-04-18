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
  ::RSA* original = test_generate(1024);
  BOOST_CHECK(original != nullptr);

  ::RSA* rotated = ::RSA_new();
  BOOST_CHECK(rotated != nullptr);

  BOOST_CHECK(dRAND_init() == 1);

  BOOST_CHECK(dRSA_rotate(rotated,
                          ::BN_num_bits(original->n),
                          (const unsigned char*)seed,
                          ::strlen(seed)) > 0);

  ::RSA_free(rotated);

  BOOST_CHECK(dRAND_clean() == 1);

  ::RSA_free(original);
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
