#include "comet.hh"

#include <comet/bn.hh>
#include <comet/rand.hh>

#include <openssl/bn.h>
#include <openssl/rand.h>

/*-----.
| Seed |
`-----*/

void
test_seed()
{
  const char* seed =
    "God exists since mathematics is consistent, "
    "and the Devil exists since we cannot prove it.";

  RAND_cleanup();
  RAND_set_rand_method(&comet::RAND_method);
  comet::RAND_display();

  RAND_cleanup();
  RAND_set_rand_method(&comet::RAND_method);
  RAND_seed(seed, ::strlen(seed));
  comet::RAND_display();

  RAND_cleanup();
  RAND_set_rand_method(&comet::RAND_method);
  comet::RAND_seed(seed, ::strlen(seed));
  comet::RAND_display();

  exit(0); // XXX
}

/*---------------.
| Generate Prime |
`---------------*/

void
test_generate_prime()
{
  // Undeterministically randomly generate numbers should should therefore
  // all be different, with high probability.
  {
    ::BIGNUM* n1 = BN_new();
    BOOST_CHECK_EQUAL(comet::BN_generate_prime_ex(n1, 1024, 0,
                                                  NULL, NULL, NULL),
                      1);

    ::BIGNUM* n2 = BN_new();
    BOOST_CHECK_EQUAL(comet::BN_generate_prime_ex(n2, 1024, 0,
                                                  NULL, NULL, NULL),
                      1);

    ::BIGNUM* n3 = BN_new();
    BOOST_CHECK_EQUAL(comet::BN_generate_prime_ex(n3, 1024, 0,
                                                  NULL, NULL, NULL),
                      1);

    BOOST_CHECK(::BN_cmp(n1, n2) != 0);
    BOOST_CHECK(::BN_cmp(n1, n3) != 0);
    BOOST_CHECK(::BN_cmp(n2, n3) != 0);
  }

  // Generate numbers in a deterministic (but probabilist) way, by resetting the
  // seed for every random generation.
  //
  // Since based on the same seed, all the generated numbers should be equal.
  {
    const char* seed =
      "God exists since mathematics is consistent, "
      "and the Devil exists since we cannot prove it.";

    ::BIGNUM* n1 = BN_new();
    RAND_set_rand_method(&comet::RAND_method);
    comet::RAND_cleanup();
    RAND_set_rand_method(&comet::RAND_method);
    comet::RAND_seed(seed, ::strlen(seed));
    BOOST_CHECK_EQUAL(comet::BN_generate_prime_ex(n1, 1024, 0,
                                                  NULL, NULL, NULL),
                      1);

    ::BIGNUM* n2 = BN_new();
    //RAND_set_rand_method(&comet::RAND_method);
    //comet::RAND_cleanup();
    //RAND_set_rand_method(&comet::RAND_method);
    comet::RAND_seed(seed, ::strlen(seed));
    BOOST_CHECK_EQUAL(comet::BN_generate_prime_ex(n2, 1024, 0,
                                                  NULL, NULL, NULL),
                      1);

    ::BIGNUM* n3 = BN_new();
    //RAND_set_rand_method(&comet::RAND_method);
    //comet::RAND_cleanup();
    //RAND_set_rand_method(&comet::RAND_method);
    comet::RAND_seed(seed, ::strlen(seed));
    BOOST_CHECK_EQUAL(comet::BN_generate_prime_ex(n3, 1024, 0,
                                                  NULL, NULL, NULL),
                      1);

    // XXX
    comet::BN_display(n1);
    comet::BN_display(n2);
    comet::BN_display(n3);

    BOOST_CHECK(::BN_cmp(n1, n2) == 0);
    BOOST_CHECK(::BN_cmp(n1, n3) == 0);
    BOOST_CHECK(::BN_cmp(n2, n3) == 0);
  }

  // XXX test with safe
}

/*-----.
| Main |
`-----*/

bool
test()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("rand");

  suite->add(BOOST_TEST_CASE(test_seed));
  suite->add(BOOST_TEST_CASE(test_generate_prime));

  boost::unit_test::framework::master_test_suite().add(suite);

  return (true);
}

int
main(int argc,
     char** argv)
{
  return (boost::unit_test::unit_test_main(test, argc, argv));
}
