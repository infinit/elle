#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE assert
#include <boost/test/unit_test.hpp>

#include <elle/assert.hh>
#include <elle/printf.hh>

BOOST_AUTO_TEST_CASE(success)
{
  ELLE_ASSERT(true);
}

BOOST_AUTO_TEST_CASE(failure)
{
  BOOST_CHECK_THROW(ELLE_ASSERT(false), elle::AssertError);
}

BOOST_AUTO_TEST_CASE(success_eq)
{
  ELLE_ASSERT_EQ(42, 42);
}

BOOST_AUTO_TEST_CASE(failure_eq)
{
  BOOST_CHECK_THROW(ELLE_ASSERT_EQ(42, 51), elle::AssertError);
}
