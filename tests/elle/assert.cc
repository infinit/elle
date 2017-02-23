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

static
void
value()
{
  {
    auto v = std::make_unique<int>(1);
    // Check ENFORCE returns the tested value.
    ELLE_ENFORCE(v).reset(new int(2));
    BOOST_TEST(*v == 2);
  }
  // ENFORCE forwards rvalues as such.
  {
    auto p = ELLE_ENFORCE(std::make_unique<int>(42));
    BOOST_TEST(*p == 42);
  }
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();

  suite.add(BOOST_TEST_CASE(success));
  suite.add(BOOST_TEST_CASE(failure));
  suite.add(BOOST_TEST_CASE(failure2));
  suite.add(BOOST_TEST_CASE(success_eq));
  suite.add(BOOST_TEST_CASE(failure_eq));
  suite.add(BOOST_TEST_CASE(value));
}
