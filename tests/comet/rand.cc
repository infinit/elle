#include "comet.hh"

#include <comet/bn.hh>
#include <comet/rand.hh>

#include <openssl/bn.h>
#include <openssl/rand.h>

/*------.
| Basic |
`------*/

static
void
test_basic()
{
  BOOST_CHECK(::dRAND_init() == 1);

  // Note that the result of the RAND_status is not checked because
  // the PNRG has not been seeded, hence does not have enough entropy.

  BOOST_CHECK(::dRAND_start() == 1);
  {
    assert(::RAND_get_rand_method() == &::dRAND_method);

    ::RAND_status();
    ::RAND_status();

    ::dRAND_reset();

    ::RAND_status();
  }
  BOOST_CHECK(::dRAND_stop() == 1);

  ::RAND_status();

  BOOST_CHECK(::dRAND_clean() == 1);
}

/*-----.
| Seed |
`-----*/

static
void
test_seed()
{
  const char* seed =
    "Sir, an equation has no meaning for me "
    "unless it expresses a thought of GOD.";

  BOOST_CHECK(::dRAND_init() == 1);

  BOOST_CHECK(::dRAND_start() == 1);
  {
    assert(::RAND_get_rand_method() == &::dRAND_method);

    // Reset the random implementation and seed the random generator.
    ::dRAND_reset();
    ::RAND_seed(seed, ::strlen(seed));
    BOOST_CHECK(::RAND_status() == 1);
    char* fingerprint1 = ::dRAND_fingerprint();
    BOOST_CHECK(fingerprint1 != nullptr);

    // Re-reset the random implementation and finally re-seed it:
    // the result should be the same as the first seeding.
    ::dRAND_reset();
    ::RAND_seed(seed, ::strlen(seed));
    BOOST_CHECK(::RAND_status() == 1);
    char* fingerprint2 = ::dRAND_fingerprint();
    BOOST_CHECK(fingerprint2 != nullptr);

    BOOST_CHECK(::strcmp(fingerprint1, fingerprint2) == 0);

    ::free(fingerprint1);
    ::free(fingerprint2);
  }
  BOOST_CHECK(::dRAND_stop() == 1);

  BOOST_CHECK(::dRAND_clean() == 1);
}

/*-----.
| Main |
`-----*/

static
bool
test()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("rand");

  suite->add(BOOST_TEST_CASE(test_basic));
  suite->add(BOOST_TEST_CASE(test_seed));

  boost::unit_test::framework::master_test_suite().add(suite);

  return (true);
}

int
main(int argc,
     char** argv)
{
  return (boost::unit_test::unit_test_main(test, argc, argv));
}
