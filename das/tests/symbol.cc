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

struct Sub
  : public S
{};

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
  static_assert(
    std::is_same<
      typename symbols::Symbol_foo::attr_type<S>::type, int>::value,
    "wrong attribute type");
  BOOST_CHECK_EQUAL(symbols::foo.attr_get(s)++, 0);
  BOOST_CHECK_EQUAL(symbols::foo.attr_get(s), 1);
  BOOST_CHECK_EQUAL(symbols::bar.attr_get(static_cast<const S&>(s)), "");
  static_assert(
    std::is_same<
      typename symbols::Symbol_bar::attr_type<S>::type, std::string>::value,
    "wrong attribute type");
  BOOST_CHECK(decltype(symbols::foo)::attr_has<S>());
  BOOST_CHECK(!no_such_attribute<decltype(symbols::foo)>(s, 0));
  BOOST_CHECK(!decltype(symbols::baz)::attr_has<S>());
  BOOST_CHECK(no_such_attribute<decltype(symbols::baz)>(s, 0));
  Sub sub;
  BOOST_CHECK_EQUAL(symbols::foo.attr_get(sub), 0);
}

struct M
{
  void foo(int i = 42);
};

static
void
methods()
{
  static_assert(symbols::foo.method_has<M>(), "method_has error");
  static_assert(symbols::foo.method_has<M, int>(), "method_has error");
  static_assert(!symbols::foo.method_has<M, int, int>(), "method_has error");
  static_assert(
    std::is_same<
      decltype(symbols::foo)::method_type<M>::type, void>::value,
    "method_type error");
  static_assert(
    std::is_same<
      decltype(symbols::foo)::method_type<M, int>::type, void>::value,
    "method_type error");
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(attributes), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(methods), 0, valgrind(1));
}
