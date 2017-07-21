#define BOOST_TEST_MODULE assert

#include <elle/assert.hh>
#include <elle/test.hh>

BOOST_AUTO_TEST_CASE(success)
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

BOOST_AUTO_TEST_CASE(failure)
{
  BOOST_CHECK_THROW(ELLE_ASSERT(false), elle::AssertError);
}

BOOST_AUTO_TEST_CASE(failure2)
{
  BOOST_CHECK_THROW(an_other_exception(), elle::AssertError);
}

BOOST_AUTO_TEST_CASE(success_eq)
{
  ELLE_ASSERT_EQ(42, 42);
}

BOOST_AUTO_TEST_CASE(failure_eq)
{
  BOOST_CHECK_THROW(ELLE_ASSERT_EQ(42, 51), elle::AssertError);
}

BOOST_AUTO_TEST_CASE(value)
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
