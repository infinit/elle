#include <ostream>

#include <elle/printf.hh>
#include <elle/test.hh>

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

namespace detail
{
  struct foo
  {
    int i;
  };

  std::ostream&
  operator <<(std::ostream& out, foo const& f);

  std::ostream&
  operator <<(std::ostream& out, foo const& f)
  {
    return out << "foo(i = " << f.i << ")";
  }
}

static
void
scoped()
{
  std::string str = elle::sprintf("%s", detail::foo{5});
  BOOST_CHECK_EQUAL(str, "foo(i = 5)");
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(test_string));
  suite.add(BOOST_TEST_CASE(scoped));
  suite.add(BOOST_TEST_CASE(test_too_few));
  suite.add(BOOST_TEST_CASE(test_empty));
  suite.add(BOOST_TEST_CASE(test_too_many));
  suite.add(BOOST_TEST_CASE(test_no_param));
}

