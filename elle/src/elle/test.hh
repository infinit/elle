#ifndef ELLE_TEST_HH
# define ELLE_TEST_HH

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

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

# ifdef INFINIT_WINDOWS
#  include <winsock2.h>
# endif
# include <boost/test/unit_test.hpp>

static std::string test_binary;

# if defined(BOOST_TEST_DYN_LINK)

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
# endif

# define ELLE_TEST_PROTOTYPE_HELPER(R, Data, I, Elem)                   \
  BOOST_PP_COMMA_IF(I)                                                  \
  BOOST_PP_TUPLE_ELEM(0, Elem) BOOST_PP_TUPLE_ELEM(1, Elem)             \

# define ELLE_TEST_PROTOTYPE(Args)                                      \
  BOOST_PP_SEQ_FOR_EACH_I(ELLE_PP_PROTOTYPE_HELPER, _, Args)            \

# define ELLE_TEST_CALL_HELPER(R, Data, I, Elem)                        \
  BOOST_PP_COMMA_IF(I)                                                  \
  BOOST_PP_TUPLE_ELEM(1, Elem)                                          \

# define ELLE_TEST_CALL(Args)                                           \
  BOOST_PP_SEQ_FOR_EACH_I(ELLE_PP_CALL_HELPER, _, Args)                 \

# define ELLE_TEST_SCHEDULED(...)                                       \
  ELLE_TEST_SCHEDULED_SEQ(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))        \

# define ELLE_TEST_SCHEDULED_SEQ(Seq)                                   \
  ELLE_TEST_SCHEDULED_HELPER(BOOST_PP_SEQ_HEAD(Seq),                    \
                             BOOST_PP_SEQ_TAIL(Seq))                    \

# define ELLE_TEST_SCHEDULED_HELPER(Name, Args)                       \
static                                                                \
void                                                                  \
BOOST_PP_CAT(Name,_impl)(ELLE_TEST_PROTOTYPE(Args));                  \
                                                                      \
static                                                                \
void                                                                  \
Name(ELLE_TEST_PROTOTYPE(Args))                                       \
{                                                                     \
  reactor::Scheduler sched;                                           \
  reactor::Thread main(                                               \
    sched, "main",                                                    \
    [&]                                                               \
    {                                                                 \
      BOOST_PP_CAT(Name,_impl)(ELLE_TEST_CALL(Args));                 \
    });                                                               \
  sched.run();                                                        \
}                                                                     \
                                                                      \
static                                                                \
void                                                                  \
BOOST_PP_CAT(Name, _impl)(ELLE_TEST_PROTOTYPE(Args))                  \

#define ELLE_TEST_SCHEDULED_THROWS(Name, _exception_type_) \
static                                                     \
void                                                       \
Name##_impl();                                             \
                                                           \
static                                                     \
void                                                       \
Name()                                                     \
{                                                          \
  reactor::Scheduler sched;                                \
  reactor::Thread main(                                    \
    sched, "main",                                         \
    [&]                                                    \
    {                                                      \
      Name##_impl();                                       \
    });                                                    \
  BOOST_CHECK_THROW(sched.run(), _exception_type_);        \
}                                                          \
                                                           \
static                                                     \
void                                                       \
Name##_impl()                                              \

#endif
