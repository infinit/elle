#include <elle/Backtrace.hh>
#include <elle/test.hh>

using elle::Backtrace;

#if defined INFINIT_WINDOWS || defined INFINIT_ANDROID || defined NO_EXECINFO

/*---------------------.
| Backtrace disabled.  |
`---------------------*/

static
void
dummy()
{
  BOOST_CHECK(true);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(dummy));
}

#else

/*--------------------.
| Backtrace enabled.  |
`--------------------*/

static
void
test_backtrace_empty()
{
  Backtrace empty;
  BOOST_CHECK(empty.frames().empty());
}

// Pacify -Wmissing-declarations.
Backtrace foo();
Backtrace bar();
Backtrace baz();
Backtrace quux();

ELLE_COMPILER_ATTRIBUTE_NO_INLINE
Backtrace
quux()
{
  return Backtrace::current();
}

ELLE_COMPILER_ATTRIBUTE_NO_INLINE
Backtrace
baz()
{
  return quux();
}

ELLE_COMPILER_ATTRIBUTE_NO_INLINE
Backtrace
bar()
{
  return baz();
}

ELLE_COMPILER_ATTRIBUTE_NO_INLINE
Backtrace
foo()
{
  return bar();
}

static
void
test_backtrace()
{
  auto bt = foo();
  BOOST_TEST(bt.frames().size() >= 4);
  auto sf = bt.frames().begin();
  BOOST_TEST(sf->symbol == "quux()");
  ++sf;
  BOOST_TEST(sf->symbol == "baz()");
  ++sf;
  BOOST_TEST(sf->symbol == "bar()");
  ++sf;
  BOOST_TEST(sf->symbol == "foo()");
  ++sf;
}

static
void
test_strip_base()
{
  auto bt = foo();
  bt.strip_base(Backtrace::current());
  BOOST_TEST(bt.frames().size() <= 5);
  BOOST_TEST(bt.frames().front().symbol == "quux()");
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(test_backtrace_empty));
  suite.add(BOOST_TEST_CASE(test_backtrace));
  suite.add(BOOST_TEST_CASE(test_strip_base));
}
#endif
