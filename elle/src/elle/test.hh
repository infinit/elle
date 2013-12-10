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

# include <boost/filesystem.hpp>

# include <elle/Exception.hh>

# ifdef ELLE_TEST_MODULE
#  define BOOST_TEST_MODULE ELLE_TEST_MODULE
# endif

# ifdef INFINIT_WINDOWS
#  include <winsock2.h>
# endif
# include <boost/test/unit_test.hpp>

# if defined(BOOST_TEST_DYN_LINK)

static boost::filesystem::path test_binary;

#  define ELLE_TEST_SUITE()                             \
static                                                  \
void                                                    \
_test_suite();                                          \
                                                        \
bool                                                    \
init_unit_test_suite();                                 \
bool                                                    \
init_unit_test_suite()                                  \
{                                                       \
  try                                                   \
  {                                                     \
    _test_suite();                                      \
  }                                                     \
  catch (...)                                           \
  {                                                     \
    throw boost::unit_test::framework::setup_error      \
      (elle::exception_string());                       \
  }                                                     \
  return true;                                          \
}                                                       \
                                                        \
int                                                     \
main(int ac, char** av)                                 \
{                                                       \
  test_binary = av[0];                                  \
  return ::boost::unit_test::unit_test_main(            \
    &init_unit_test_suite, ac, av);                     \
}                                                       \
                                                        \
                                                        \
static                                                  \
void                                                    \
_test_suite()

# elif defined(BOOST_TEST_STATIC_LINK)

#  define ELLE_TEST_SUITE()                             \
static                                                  \
void                                                    \
_test_suite();                                          \
                                                        \
boost::unit_test::test_suite*                           \
init_unit_test_suite(int, char**);                      \
boost::unit_test::test_suite*                           \
init_unit_test_suite(int ac, char** av)                 \
{                                                       \
  test_binary = av[0];                                  \
  try                                                   \
  {                                                     \
    _test_suite();                                      \
  }                                                     \
  catch (...)                                           \
  {                                                     \
    throw boost::unit_test::framework::setup_error      \
      (elle::exception_string());                       \
  }                                                     \
  return nullptr;                                       \
}                                                       \
                                                        \
static                                                  \
void                                                    \
_test_suite()                                           \

# else
#  error "please define BOOST_TEST_DYN_LINK or BOOST_TEST_STATIC_LINK"
#endif


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
