#ifndef ELLE_TEST_HH
# define ELLE_TEST_HH

/// This header includes boost Unit Test Framework and provides a simple macro
/// to customize the creation of your test suite.
///
/// There are two supported ways to define your tests:
/// 1. Using BOOST_AUTO_TEST_CASE and generated test suite:
/// {{{
///     #define ELLE_TEST_MODULE "My module"
///     #include <elle/test.hh>
///     BOOST_AUTO_TEST_CASE(SimpleCase) { /* Test your code here */ }
///.}}}
///
/// 2. By Defining your own test suite:
/// {{{
///     #include <elle/test.hh>
///     ELLE_TEST_SUITE() { /* Create and initialize your test suite here */ }
/// }}}
///

# include <elle/Exception.hh>

# ifdef ELLE_TEST_MODULE
#  define BOOST_TEST_MODULE ELLE_TEST_MODULE
# endif

# ifndef BOOST_TEST_MODULE
#  define BOOST_TEST_NO_MAIN
# endif

# ifndef BOOST_TEST_ALTERNATIVE_INIT_API
#  define BOOST_TEST_ALTERNATIVE_INIT_API 1
# endif
# ifdef INFINIT_WINDOWS
#  include <winsock2.h>
# endif
# include <boost/test/unit_test.hpp>


# define ELLE_TEST_SUITE()                                                    \
static                                                                        \
void                                                                          \
_test_suite();                                                                \
                                                                              \
static                                                                        \
bool                                                                          \
test_suite()                                                                  \
{                                                                             \
  try { _test_suite(); }                                                      \
  catch (...) {                                                               \
    std::cerr << "Couldn't prepare the test suite in "                        \
              << __FILE__ << ": " << elle::exception_string() << std::endl;   \
    return false;                                                             \
  }                                                                           \
  return true;                                                                \
}                                                                             \
                                                                              \
int                                                                           \
main(int ac, char** av)                                                       \
{                                                                             \
  return ::boost::unit_test::unit_test_main(&test_suite, ac, av);             \
}                                                                             \
                                                                              \
static                                                                        \
void                                                                          \
_test_suite()                                                                 \
/**/

#define ELLE_TEST_SCHEDULED(Name)                       \
static                                                  \
void                                                    \
Name##_impl();                                          \
                                                        \
static                                                  \
void                                                    \
Name()                                                  \
{                                                       \
  reactor::Scheduler sched;                             \
  reactor::Thread main(                                 \
    sched, "main",                                      \
    [&]                                                 \
    {                                                   \
      Name##_impl();                                    \
    });                                                 \
  sched.run();                                          \
}                                                       \
                                                        \
static                                                  \
void                                                    \
Name##_impl()                                           \

#endif
