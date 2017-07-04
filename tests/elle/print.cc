#include <ostream>

#include <elle/print.hh>
#include <elle/test.hh>

static
void
empty()
{
  BOOST_CHECK_EQUAL(elle::print(""), "");
}

static
void
no_param()
{
  BOOST_CHECK_EQUAL(elle::print(" foo bar "), " foo bar ");
}

static
void
litteral()
{
  BOOST_CHECK_EQUAL(elle::print("\\}\\{\\{\\}"), "}{{}");
}

static
void
string()
{
  BOOST_CHECK_EQUAL(elle::print("foo{}baz", "bar"), "foobarbaz");
}

static
void
null_string()
{
  char const* s1 = nullptr;
  char* s2 = nullptr;
  BOOST_CHECK_EQUAL(elle::print("empty: {}{}", s1, s2), "empty: ");
}

static
void
c_string()
{
  char const* s1 = "foo";
  char* s2 = const_cast<char*>("bar");
  BOOST_CHECK_EQUAL(elle::print("{} {}", s1, s2), "foo bar");
}

static
void
too_few()
{
  BOOST_CHECK_THROW(elle::print("{}{}", "foo"), std::exception);
}

namespace detail
{
  struct foo
  {
    int i;
  };

  static
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
  std::string str = elle::print("{}", detail::foo{5});
  BOOST_CHECK_EQUAL(str, "foo(i = 5)");
}

static
void
boolean()
{
  BOOST_CHECK_EQUAL(elle::print("{} {}", true, false), "true false");
}

static
void
indexed()
{
  BOOST_CHECK_EQUAL(elle::print("{2} {1} {0}", 0, 1, 2), "2 1 0");
}

static
void
named()
{
  BOOST_CHECK_EQUAL(elle::print("{n} * {n} = {squared}",
                                {{"n", 4}, {"squared", 16}}),
                    "4 * 4 = 16");
}

static
void
conditional()
{
  BOOST_CHECK_EQUAL(
    elle::print(
      "{zero?oops} - {one?one} - {empty?oops} - {filled?content: {filled}}",
      {{"zero", 0}, {"one", 1}, {"empty", ""}, {"filled", "yes"}}),
     " - one -  - content: yes");
}

static
void
conditional_positional()
{
  BOOST_CHECK_EQUAL(
    elle::print(
      "{?{}, }{?{}, }{4?{5}, }{6?{7}, }",
      false, 0, true, 1, true, 2, false, 4),
     "1, 2, ");
  BOOST_CHECK_EQUAL(
    elle::print(
      "{yes?success}{no?fail}",
      {
        {"yes", 1},
        {"no", 0},
      }),
    "success");
}

namespace
{
  struct Foo {};
  std::ostream& operator << (std::ostream& o, Foo)
  {
    return o << (elle::repr(o) ? "Verbose" : "Silent");
  }

  void
  legacy()
  {
#define CHECK(Out, ...)                         \
    BOOST_TEST(Out == elle::print(__VA_ARGS__))
    CHECK("86", "%x", 134);
    CHECK("134", "%s", 134);
    CHECK("%", "%%");
    CHECK("86 134%", "%x %s%%", 134, 134);
    CHECK("Verbose Silent", "%r %s", Foo{}, Foo{});
#undef CHECK
  }
}


ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(empty));
  suite.add(BOOST_TEST_CASE(no_param));
  suite.add(BOOST_TEST_CASE(litteral));
  suite.add(BOOST_TEST_CASE(string));
  suite.add(BOOST_TEST_CASE(null_string));
  suite.add(BOOST_TEST_CASE(c_string));
  suite.add(BOOST_TEST_CASE(too_few));
  suite.add(BOOST_TEST_CASE(scoped));
  suite.add(BOOST_TEST_CASE(boolean));
  suite.add(BOOST_TEST_CASE(indexed));
  suite.add(BOOST_TEST_CASE(named));
  suite.add(BOOST_TEST_CASE(conditional));
  suite.add(BOOST_TEST_CASE(conditional_positional));
  suite.add(BOOST_TEST_CASE(legacy));
}
