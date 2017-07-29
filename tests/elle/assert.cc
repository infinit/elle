#define BOOST_TEST_MODULE assert

#include <boost/algorithm/cxx11/any_of.hpp>

#include <elle/assert.hh>
#include <elle/test.hh>

BOOST_AUTO_TEST_CASE(success)
{
  ELLE_ASSERT(true);
  try
  {
    throw std::runtime_error("foo");
  }
  catch(std::exception const& e)
  {}
  ELLE_ASSERT_NO_OTHER_EXCEPTION
}

static void an_other_exception()
{
  try
  {
    throw std::bad_alloc();
  }
  catch(std::bad_cast const& e)
  {}
  ELLE_ASSERT_NO_OTHER_EXCEPTION
}

BOOST_AUTO_TEST_CASE(failure)
{
  BOOST_CHECK_THROW(ELLE_ASSERT(false), elle::AssertError);
}

BOOST_AUTO_TEST_CASE(failure2)
{
  BOOST_CHECK_THROW(an_other_exception(), elle::AssertError);
}

BOOST_AUTO_TEST_CASE(success_eq)
{
  ELLE_ASSERT_EQ(42, 42);
}

BOOST_AUTO_TEST_CASE(failure_eq)
{
  BOOST_CHECK_THROW(ELLE_ASSERT_EQ(42, 51), elle::AssertError);
}

BOOST_AUTO_TEST_CASE(value)
{
  {
    auto v = std::make_unique<int>(1);
    // Check ENFORCE returns the tested value.
    ELLE_ENFORCE(v).reset(new int(2));
    BOOST_TEST(*v == 2);
  }
  // ENFORCE forwards rvalues as such.
  {
    auto p = ELLE_ENFORCE(std::make_unique<int>(42));
    BOOST_TEST(*p == 42);
  }
}

// Pacify -Wmissing-declarations.  Avoid unnamed namespace, it changes
// the names.
void f0();
void f1();
void f2();
void f3();

#define NO_INLINE ELLE_COMPILER_ATTRIBUTE_NO_INLINE

NO_INLINE void f0() { ELLE_ASSERT_EQ(42, 51); }; auto const line = __LINE__;
NO_INLINE void f1() { f0(); }
NO_INLINE void f2() { f1(); }
NO_INLINE void f3() { f2(); }

BOOST_AUTO_TEST_CASE(abort_callbacks)
{
  using boost::algorithm::any_of;

  auto err = elle::AssertError{};
  elle::on_abort([&err](elle::AssertError const& e)
    {
      err = e;
    });
  BOOST_CHECK_THROW(f3(), elle::AssertError);
  BOOST_TEST(err.what() ==
             elle::print("assertion '42 == 51 is false: (42=42, 51=51)' failed at %s:%s",
                         __FILE__, line));
  // At least clang does not seem to obey the NO_INLINE, and shows
  // only f0.
  BOOST_TEST_MESSAGE("Backtrace: " << *err.backtrace());
#if ELLE_HAVE_BACKTRACE
  BOOST_TEST(any_of(err.backtrace()->frames(),
                    [](auto const& frame)
                    {
                      return frame.symbol == "f0()";
                    }));
#endif
}
