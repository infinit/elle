#include <elle/assert.hh>
#include <elle/test.hh>

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

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();

  suite.add(BOOST_TEST_CASE(success));
  suite.add(BOOST_TEST_CASE(failure));
  suite.add(BOOST_TEST_CASE(success_eq));
  suite.add(BOOST_TEST_CASE(failure_eq));
}
