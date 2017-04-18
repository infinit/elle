#include <ostream>

#include <elle/printf.hh>
#include <elle/test.hh>

static
void
empty()
{
  BOOST_CHECK_EQUAL(elle::sprintf(""), "");
}

static
void
no_param()
{
  BOOST_CHECK_EQUAL(elle::sprintf(" foo bar "), " foo bar ");
}

static
void
string()
{
  BOOST_CHECK_EQUAL(elle::sprintf("foo%sbaz", "bar"), "foobarbaz");
}

static
void
null_string()
{
  char const* s1 = nullptr;
  char* s2 = nullptr;
  BOOST_CHECK_EQUAL(elle::sprintf("empty: %s%s", s1, s2), "empty: ");
}

static
void
c_string()
{
  char const* s1 = "foo";
  char* s2 = const_cast<char*>("bar");
  BOOST_TEST(elle::sprintf("%s %s", s1, s2) == "foo bar");
}

static
void
too_many()
{
  BOOST_CHECK_THROW(elle::sprintf("%s", "foo", "bar"), std::exception);
}

static
void
too_few()
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

static
void
boolean()
{
  BOOST_CHECK_EQUAL(elle::sprintf("%s %s", true, false), "true false");
}

static
void
foo(int, float)
{}

static
void
function_pointer()
{
  BOOST_CHECK_EQUAL(elle::sprintf("%s", &foo), "<function>");
}

static
void
pointers()
{
  auto u = std::make_unique<int>(107);
  auto s = std::make_shared<int>(107);
  auto w = std::weak_ptr<int>(s);
  auto a = std::ambivalent_ptr<int>(s);
  BOOST_CHECK_EQUAL(elle::sprintf("%s", u), "107");
  BOOST_CHECK_EQUAL(elle::sprintf("%s", s), "107");
  BOOST_CHECK_EQUAL(elle::sprintf("%s", w), "107");
  BOOST_CHECK_EQUAL(elle::sprintf("%s", a), "107");
  u.reset();
  s.reset();
  a.reset();
  BOOST_CHECK_EQUAL(elle::sprintf("%s", u), "nullptr");
  BOOST_CHECK_EQUAL(elle::sprintf("%s", s), "nullptr");
  BOOST_CHECK_EQUAL(elle::sprintf("%s", w), "nullptr");
  BOOST_CHECK_EQUAL(elle::sprintf("%s", a), "nullptr");
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(string));
  suite.add(BOOST_TEST_CASE(null_string));
  suite.add(BOOST_TEST_CASE(c_string));
  suite.add(BOOST_TEST_CASE(scoped));
  suite.add(BOOST_TEST_CASE(too_few));
  suite.add(BOOST_TEST_CASE(empty));
  suite.add(BOOST_TEST_CASE(too_many));
  suite.add(BOOST_TEST_CASE(no_param));
  suite.add(BOOST_TEST_CASE(boolean));
  suite.add(BOOST_TEST_CASE(function_pointer));
  suite.add(BOOST_TEST_CASE(pointers));
}
