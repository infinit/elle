#define BOOST_TEST_DYN_LINK

#include <elle/assert.hh>
#include <elle/printf.hh>

#include <boost/test/unit_test.hpp>

static
void
success()
{
  ELLE_ASSERT(true);
}

static
void
failure()
{
  BOOST_CHECK_THROW(ELLE_ASSERT(false), elle::AssertError);
}

static
void
success_eq()
{
  ELLE_ASSERT_EQ(42, 42);
}

static
void
failure_eq()
{
  BOOST_CHECK_THROW(ELLE_ASSERT_EQ(42, 51), elle::AssertError);
}

static
bool
test_suite()
{
  boost::unit_test::test_suite* basics = BOOST_TEST_SUITE("Basics");
  boost::unit_test::framework::master_test_suite().add(basics);

  basics->add(BOOST_TEST_CASE(success));
  basics->add(BOOST_TEST_CASE(failure));
  basics->add(BOOST_TEST_CASE(success_eq));
  basics->add(BOOST_TEST_CASE(failure_eq));

  return true;
}

int
main(int argc, char** argv)
{
	return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}
