#include <elle/assert.hh>
#include <elle/test.hh>

static
void
success()
{
  ELLE_ASSERT(true);
  try
  {
    throw std::runtime_error("foo");
  }
  catch(std::exception const& e)
  {}
  ELLE_ASSERT_NO_OTHER_EXCEPTION
}

static void an_other_exception()
{
  try
  {
    throw std::bad_alloc();
  }
  catch(std::bad_cast const& e)
  {}
  ELLE_ASSERT_NO_OTHER_EXCEPTION
}

static
void
failure()
{
  BOOST_CHECK_THROW(ELLE_ASSERT(false), elle::AssertError);
}

static
void
failure2()
{
  BOOST_CHECK_THROW(an_other_exception(), elle::AssertError);
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
  suite.add(BOOST_TEST_CASE(failure2));
  suite.add(BOOST_TEST_CASE(success_eq));
  suite.add(BOOST_TEST_CASE(failure_eq));
}
