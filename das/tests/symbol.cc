#include <string>

#include <elle/test.hh>

#include <das/Symbol.hh>

namespace symbols
{
  DAS_SYMBOL(foo);
  DAS_SYMBOL(bar);
  DAS_SYMBOL(baz);
  DAS_SYMBOL(quux);
}

struct S
{
  S()
    : foo(0)
  {}
  int foo;
  std::string bar;
};

template <typename S, typename T>
auto
no_such_attribute(T const& o, int i)
  -> typename std::enable_if<sizeof(S::attr_get(o)) >= 0, bool>::type
{
  return false;
}

template <typename S, typename T>
bool
no_such_attribute(T const& o, ...)
{
  return true;
}

static
void
attributes()
{
  S s;
  BOOST_CHECK_EQUAL(symbols::foo.attr_get(s), 0);
  BOOST_CHECK_EQUAL(symbols::foo.attr_get(s)++, 0);
  BOOST_CHECK_EQUAL(symbols::foo.attr_get(s), 1);
  BOOST_CHECK_EQUAL(symbols::bar.attr_get(static_cast<const S&>(s)), "");
  BOOST_CHECK(!no_such_attribute<decltype(symbols::foo)>(s, 0));
  BOOST_CHECK(no_such_attribute<decltype(symbols::baz)>(s, 0));
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(attributes), 0, valgrind(1));
}
