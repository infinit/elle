#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <elle/printf.hh>

static
void
test_empty()
{
  BOOST_CHECK_EQUAL(elle::sprintf(""), "");
}

static
void
test_no_param()
{
  BOOST_CHECK_EQUAL(elle::sprintf(" foo bar "), " foo bar ");
}

static
void
test_string()
{
  BOOST_CHECK_EQUAL(elle::sprintf("foo%sbaz", "bar"), "foobarbaz");
}

static
void
test_too_many()
{
  BOOST_CHECK_THROW(elle::sprintf("%s", "foo", "bar"), std::exception);
}

static
void
test_too_few()
{
  BOOST_CHECK_THROW(elle::sprintf("%s%s", "foo"), std::exception);
}

static
bool
test_suite()
{
  boost::unit_test::test_suite* basics = BOOST_TEST_SUITE("Basics");
  boost::unit_test::framework::master_test_suite().add(basics);
  basics->add(BOOST_TEST_CASE(test_empty));
  basics->add(BOOST_TEST_CASE(test_no_param));
  basics->add(BOOST_TEST_CASE(test_string));

  boost::unit_test::test_suite* errors = BOOST_TEST_SUITE("Errors");
  boost::unit_test::framework::master_test_suite().add(errors);
  errors->add(BOOST_TEST_CASE(test_too_many));
  errors->add(BOOST_TEST_CASE(test_too_few));

  return true;
}

int
main(int argc, char** argv)
{
  return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}
