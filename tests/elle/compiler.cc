#include <elle/test.hh>
#include <elle/compiler.hh>

#if !defined INFINIT_WINDOWS && !defined INFINIT_ANDROID
# include <execinfo.h>
#endif

static ELLE_COMPILER_ATTRIBUTE_NORETURN
void
noreturn()
{
  throw std::runtime_error("");
}

static
void
test_noreturn()
{
  try
  {
    noreturn();
  }
  catch (...)
  {}
}

ELLE_COMPILER_ATTRIBUTE_ALWAYS_INLINE inline
void
always_inline()
{
  // I have no decent idea to check if this is actually inlined. Even
  // ::backtrace is subject to offsets and won't work.
}

static
void
test_always_inline()
{
  always_inline();
}

ELLE_COMPILER_ATTRIBUTE_NO_INLINE inline
void
no_inline()
{
  // I have no decent idea to check if this is actually not inlined. Even
  // ::backtrace is subject to offsets and won't work.
}

static
void
test_no_inline()
{
  no_inline();
}

static
void
test_pretty_function()
{
  BOOST_CHECK_EQUAL(ELLE_COMPILER_PRETTY_FUNCTION,
                    "void test_pretty_function()");
}

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* exn = BOOST_TEST_SUITE("Exception");
  boost::unit_test::framework::master_test_suite().add(exn);
  exn->add(BOOST_TEST_CASE(test_noreturn));
  exn->add(BOOST_TEST_CASE(test_always_inline));
  exn->add(BOOST_TEST_CASE(test_no_inline));
  exn->add(BOOST_TEST_CASE(test_pretty_function));
}
