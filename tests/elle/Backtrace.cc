#define BOOST_TEST_MODULE Backtrace

#include <elle/Backtrace.hh>
#include <elle/test.hh>

using elle::Backtrace;

/// How the Backtrace is built.
enum Via { via_ctor, via_current };

// Pacify -Wmissing-declarations.  Avoid unnamed namespace, it changes
// the names.
Backtrace foo(Via v);
Backtrace bar(Via v);
Backtrace baz(Via v);
Backtrace qux(Via v);

ELLE_COMPILER_ATTRIBUTE_NO_INLINE
Backtrace
qux(Via via)
{
  return via == via_ctor ? Backtrace{Backtrace::now} : Backtrace::current();
}

ELLE_COMPILER_ATTRIBUTE_NO_INLINE
Backtrace
baz(Via via)
{
  return qux(via);
}

ELLE_COMPILER_ATTRIBUTE_NO_INLINE
Backtrace
bar(Via via)
{
  return baz(via);
}

ELLE_COMPILER_ATTRIBUTE_NO_INLINE
Backtrace
foo(Via via)
{
  return bar(via);
}

BOOST_AUTO_TEST_CASE(syndical_minimum)
{
  // Should not work if not available, but should not die either.
  for (auto via: {via_ctor, via_current})
    BOOST_CHECK_NO_THROW(foo(via));
}

#if ELLE_HAVE_BACKTRACE

BOOST_AUTO_TEST_CASE(test_backtrace_empty)
{
  Backtrace empty;
  BOOST_CHECK(empty.frames().empty());
}

BOOST_AUTO_TEST_CASE(test_backtrace)
{
  for (auto via: {via_ctor, via_current})
  {
    auto const bt = foo(via);
    BOOST_TEST_MESSAGE("Backtrace: " << bt);
    auto const& sf = bt.frames();
    BOOST_TEST(sf.size() >= 4);
    BOOST_TEST(sf[0].symbol == "qux(Via)");
    BOOST_TEST(sf[1].symbol == "baz(Via)");
    BOOST_TEST(sf[2].symbol == "bar(Via)");
    BOOST_TEST(sf[3].symbol == "foo(Via)");
  }
}

BOOST_AUTO_TEST_CASE(test_strip_base)
{
  for (auto via: {via_ctor, via_current})
  {
    auto bt = foo(via);
    bt.strip_base(Backtrace::current());
    BOOST_TEST_MESSAGE("Backtrace: " << bt);
    BOOST_TEST(bt.frames().size() <= 5);
    BOOST_TEST(bt.frames().front().symbol == "qux(Via)");
  }
}
#endif
