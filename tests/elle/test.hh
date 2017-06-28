#pragma once

#ifdef VALGRIND
# include <valgrind/valgrind.h>
#else
# define RUNNING_ON_VALGRIND 0
#endif

// GCC define __SANITIZE_ADDRESS__ for -fsanitize=address.
#if defined(__has_feature)
# if __has_feature(address_sanitizer)
#  define __SANITIZE_ADDRESS__ 1
# endif
#endif

#if __SANITIZE_ADDRESS__
# define RUNNING_ON_ASAN true
#else
# define RUNNING_ON_ASAN false
#endif

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

#include <elle/Error.hh>
#include <elle/log.hh>
#include <elle/reactor/scheduler.hh>

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

#include <elle/Exception.hh>

#ifdef ELLE_TEST_MODULE
# define BOOST_TEST_MODULE ELLE_TEST_MODULE
#endif

#ifdef INFINIT_WINDOWS
# include <winsock2.h>
#endif
#include <boost/test/unit_test.hpp>

static std::string test_binary;

#define ELLE_TEST_CASE(F, ...)                                          \
  boost::unit_test::make_test_case(                                     \
    F,                                                                  \
    BOOST_PP_SEQ_HEAD(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)(#F)),       \
    __FILE__, __LINE__ )                                                \

#if defined BOOST_TEST_DYN_LINK

# define ELLE_TEST_SUITE()                              \
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

# elif defined BOOST_TEST_STATIC_LINK

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

#else
# error "please define BOOST_TEST_DYN_LINK or BOOST_TEST_STATIC_LINK"
#endif

#define ELLE_TEST_PROTOTYPE_HELPER(R, Data, I, Elem)                    \
  BOOST_PP_COMMA_IF(I)                                                  \
  BOOST_PP_TUPLE_ELEM(0, Elem) BOOST_PP_TUPLE_ELEM(1, Elem)             \

#define ELLE_TEST_PROTOTYPE(Args)                                       \
  BOOST_PP_SEQ_FOR_EACH_I(ELLE_TEST_PROTOTYPE_HELPER, _, Args)          \

#define ELLE_TEST_CALL_HELPER(R, Data, I, Elem)                         \
  BOOST_PP_COMMA_IF(I)                                                  \
  BOOST_PP_TUPLE_ELEM(1, Elem)                                          \

#define ELLE_TEST_CALL(Args)                                            \
  BOOST_PP_SEQ_FOR_EACH_I(ELLE_TEST_CALL_HELPER, _, Args)               \

#define ELLE_TEST_SCHEDULED(...)                                        \
  ELLE_TEST_SCHEDULED_SEQ(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))        \

#define ELLE_TEST_SCHEDULED_SEQ(Seq)                                    \
  ELLE_TEST_SCHEDULED_HELPER(BOOST_PP_SEQ_HEAD(Seq),                    \
                             BOOST_PP_SEQ_TAIL(Seq))                    \


/*-----------------.
| Alarm handling.  |
`-----------------*/

#if defined INFINIT_WINDOWS
# define ELLE_TEST_HANDLE_SIGALRM(Sched, Name)
#else
namespace
{
  void die(int)
  {
    ELLE_LOG_COMPONENT("elle.Test");
    ELLE_ERR("failed to teardown cleanly.  Die now!");
    _exit(70); // EX_SOFTWARE
  }

  ELLE_COMPILER_ATTRIBUTE_MAYBE_UNUSED
  void alarm_handler(std::string const& name)
  {
    ELLE_LOG_COMPONENT("elle.Test");
    ELLE_ERR("test %s timeout: SIGALRM", name);
    // Give us some time to teardown properly, dump stuff etc., then
    // exit brutally.  The buildfarm has been stuck several times for
    // hours because of a signal handling that did not finish.
    signal(SIGALRM, die);
    alarm(30);
    if (auto s = elle::reactor::Scheduler::scheduler())
      s->dump_state();
    throw elle::Error("test timeout");
  }
}

# define ELLE_TEST_HANDLE_SIGALRM(Sched, Name)                  \
  Sched.signal_handle(SIGALRM, [] { alarm_handler(#Name); })
#endif



#define ELLE_TEST_SCHEDULED_HELPER(Name, Args)                        \
static                                                                \
void                                                                  \
BOOST_PP_CAT(Name,_impl)(ELLE_TEST_PROTOTYPE(Args));                  \
                                                                      \
static                                                                \
void                                                                  \
Name(ELLE_TEST_PROTOTYPE(Args))                                       \
{                                                                     \
  elle::reactor::Scheduler sched;                                     \
  ELLE_TEST_HANDLE_SIGALRM(sched, Name);                              \
  elle::reactor::Thread main(                                         \
    sched, "main",                                                    \
    [&]                                                               \
    {                                                                 \
      ELLE_LOG_COMPONENT("elle.Test");                                \
      ELLE_LOG("starting test: %s (%s)", BOOST_PP_STRINGIZE(Name),      \
               boost::unit_test::framework::current_test_case().full_name())\
        BOOST_PP_CAT(Name,_impl)(ELLE_TEST_CALL(Args));               \
    });                                                               \
  try                                                                 \
  {                                                                   \
    sched.run();                                                      \
  }                                                                   \
  catch (elle::Error const& e)                                        \
  {                                                                   \
    ELLE_LOG_COMPONENT("elle.Test");                                  \
    ELLE_ERR("exception escaped test %s: %s",                         \
             BOOST_PP_STRINGIZE(Name), e);                            \
    ELLE_ERR("%s", e.backtrace());                                    \
    throw;                                                            \
  }                                                                   \
}                                                                     \
                                                                      \
static                                                                \
void                                                                  \
BOOST_PP_CAT(Name, _impl)(ELLE_TEST_PROTOTYPE(Args))                  \

#define ELLE_TEST_SCHEDULED_THROWS(Name, _exception_type_)            \
static                                                                \
void                                                                  \
Name##_impl();                                                        \
                                                                      \
static                                                                \
void                                                                  \
Name()                                                                \
{                                                                     \
  elle::reactor::Scheduler sched;                                     \
  elle::reactor::Thread main(                                         \
    sched, "main",                                                    \
    [&]                                                               \
    {                                                                 \
      ELLE_LOG_COMPONENT("elle.Test")                                 \
      ELLE_LOG("starting test: %s (%s)", BOOST_PP_STRINGIZE(Name),     \
        boost::unit_test::framework::current_test_case().full_name());\
      Name##_impl();                                                  \
    });                                                               \
  BOOST_CHECK_THROW(sched.run(), _exception_type_);                   \
}                                                                     \
                                                                      \
static                                                                \
void                                                                  \
Name##_impl()                                                         \



// Clang defines _FORTIFY_SOURCE when given -fsanitize=address, and it
// does not like what we do here.
#if defined _FORTIFY_SOURCE
# define  ELLE_TEST_NO_MEMFRY 1
#endif

#if !defined ELLE_TEST_NO_MEMFRY

// Apple clang-703.0.29 doesn't like using an offset of size_t.
static const int _memfry_offset = 2 * sizeof(std::size_t);

void*
operator new(std::size_t n, std::nothrow_t const&) noexcept
{
  if (RUNNING_ON_VALGRIND)
    return std::malloc(n);
  else
  {
    auto chunk = reinterpret_cast<char*>(std::malloc(n + _memfry_offset));
    *reinterpret_cast<std::size_t*>(chunk) = n;
    std::memset(chunk + _memfry_offset, 0xd0, n);
    return chunk + _memfry_offset;
  }
}

void*
operator new(std::size_t n) noexcept(false)
{
  if (void* res = ::operator new(n, std::nothrow))
    return res;
  else
    throw std::bad_alloc();
}

void
operator delete(void* p) noexcept
{
  if (RUNNING_ON_VALGRIND)
    std::free(p);
  else if (p)
  {
    auto chunk = reinterpret_cast<char*>(p) - _memfry_offset;
    auto n = *reinterpret_cast<std::size_t*>(chunk);
    std::memset(chunk, 0xdf, n + _memfry_offset);
    std::free(chunk);
  }
}

#endif

#ifdef __arm__
# define ARM_FACTOR 1
#else
# define ARM_FACTOR 0
#endif

/// Scale a duration `d` by `factor` if we are instrumented (valgrind,
/// asan, etc.).
template <typename T>
static
auto
valgrind(T d, int factor = 50)
{
  auto const instrumented = RUNNING_ON_VALGRIND || RUNNING_ON_ASAN;
  return d * (instrumented ? factor : 1) * (ARM_FACTOR ? factor : 1);
}
