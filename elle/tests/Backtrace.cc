#include <elle/Backtrace.hh>
#include <elle/test.hh>

using elle::Backtrace;

#if !defined INFINIT_WINDOWS && !defined INFINIT_ANDROID

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
  Backtrace bt(foo());
  BOOST_CHECK_GE(bt.frames().size(), 4);
  auto sf = bt.frames().begin();
  BOOST_CHECK_EQUAL(sf->symbol, "quux()");
  ++sf;
  BOOST_CHECK_EQUAL(sf->symbol, "baz()");
  ++sf;
  BOOST_CHECK_EQUAL(sf->symbol, "bar()");
  ++sf;
  BOOST_CHECK_EQUAL(sf->symbol, "foo()");
  ++sf;
}

static
void
test_strip_base()
{
  Backtrace bt(foo());
  bt.strip_base(Backtrace::current());
  BOOST_CHECK_LE(bt.frames().size(), 5);
  BOOST_CHECK_EQUAL(bt.frames().front().symbol, "quux()");
}

#else

static
void
dummy()
{
  BOOST_CHECK(true);
}

#endif

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
#if !defined INFINIT_WINDOWS && !defined INFINIT_ANDROID
  suite.add(BOOST_TEST_CASE(test_backtrace_empty));
  suite.add(BOOST_TEST_CASE(test_backtrace));
  suite.add(BOOST_TEST_CASE(test_strip_base));
#else
  suite.add(BOOST_TEST_CASE(dummy));
#endif
}
