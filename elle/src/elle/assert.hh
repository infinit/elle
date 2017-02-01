#pragma once

#include <string>
#include <stdexcept>

#include <elle/compiler.hh>

namespace elle ELLE_API
{
  /// Print the message and abort program execution.
  ELLE_COMPILER_ATTRIBUTE_NORETURN
  void
  _abort(std::string const& msg,
         char const* file,
         int line);

  /// Abort the program. Flags unreachable code.
  ELLE_COMPILER_ATTRIBUTE_NORETURN
  void
  unreachable();
}

#include <elle/compiler.hh>
#include <elle/printf.hh>
#include <elle/types.hh>

namespace elle
{
  /// Exception thrown when an assertion is unmet.
  ///
  /// @note You should never catch directly `AssertError`, nor its base class
  /// `std::exception`, but in the main function of the program.
  class ELLE_API AssertError:
    public std::exception
  {
  private:
    std::string _what;
  public:
    AssertError(char const* condition,
                char const* file,
                Size line) throw();

    virtual
    const char*
    what() const throw();
  };
}

#define ELLE_ABORT(...)                                               \
  ::elle::_abort(elle::sprintf(__VA_ARGS__), __FILE__, __LINE__)

/// Enforce a condition is true (always present in the code)
/// @see ELLE_ASSERT for debug only assertions.
#define ELLE_ENFORCE(_condition_)                                           \
  ::elle::_elle_assert(_condition_, #_condition_, __FILE__, __LINE__)

#define ELLE_ENFORCE_EQ(A, B)                                               \
  ::elle::_assert_eq(A, B, #A, #B, __FILE__, __LINE__)

#define ELLE_ENFORCE_NEQ(A, B)                                              \
  ::elle::_assert_neq(A, B, #A, #B, __FILE__, __LINE__)

#define ELLE_ENFORCE_GT(A, B)                                               \
  ::elle::_assert_gt(A, B, #A, #B, __FILE__, __LINE__)

#define ELLE_ENFORCE_GTE(A, B)                                              \
  ::elle::_assert_gte(A, B, #A, #B, __FILE__, __LINE__)

#define ELLE_ENFORCE_LT(A, B)                                               \
  ::elle::_assert_lt(A, B, #A, #B, __FILE__, __LINE__)

#define ELLE_ENFORCE_LTE(A, B)                                              \
  ::elle::_assert_lte(A, B, #A, #B, __FILE__, __LINE__)

#define ELLE_ENFORCE_CONTAINS(C, E)                                         \
  ::elle::_assert_contains(C, E, #C, #E, __FILE__, __LINE__)

#define ELLE_ENFORCE_NCONTAINS(C, E)                                        \
  ::elle::_assert_ncontains(C, E, #C, #E, __FILE__, __LINE__)

/// Use after the last catch close to enforce no other exception is caught
#define ELLE_ENFORCE_NO_OTHER_EXCEPTION                                \
  catch(...)                                                            \
  {                                                                     \
    ::elle::_elle_assert(                                               \
      false, elle::sprintf("Expected no more exception, got '%s'",      \
                           elle::exception_string()),                   \
      __FILE__, __LINE__);                                              \
  }

#if defined DEBUG || !defined NDEBUG
/// Throw if the condition is unmet.
# define ELLE_ASSERT(_condition_) ELLE_ENFORCE(_condition_)
# define ELLE_ASSERT_EQ(A, B) ELLE_ENFORCE_EQ(A, B)
# define ELLE_ASSERT_NEQ(A, B) ELLE_ENFORCE_NEQ(A, B)
# define ELLE_ASSERT_GT(A, B) ELLE_ENFORCE_GT(A, B)
# define ELLE_ASSERT_GTE(A, B) ELLE_ENFORCE_GTE(A, B)
# define ELLE_ASSERT_LT(A, B) ELLE_ENFORCE_LT(A, B)
# define ELLE_ASSERT_LTE(A, B) ELLE_ENFORCE_LTE(A, B)
# define ELLE_ASSERT_CONTAINS(C, E) ELLE_ENFORCE_CONTAINS(C, E)
# define ELLE_ASSERT_NCONTAINS(C, E) ELLE_ENFORCE_NCONTAINS(C, E)
# define ELLE_ASSERT_NO_OTHER_EXCEPTION ELLE_ENFORCE_NO_OTHER_EXCEPTION
#else
# define ELLE_ASSERT(_condition_) ((void) 0)
# define ELLE_ASSERT_EQ(A, B) ELLE_ASSERT(true)
# define ELLE_ASSERT_NEQ(A, B) ELLE_ASSERT(true)
# define ELLE_ASSERT_GT(A, B) ELLE_ASSERT(true)
# define ELLE_ASSERT_GTE(A, B) ELLE_ASSERT(true)
# define ELLE_ASSERT_LT(A, B) ELLE_ASSERT(true)
# define ELLE_ASSERT_LTE(A, B) ELLE_ASSERT(true)
# define ELLE_ASSERT_CONTAINS(C, E) ELLE_ASSERT(true)
# define ELLE_ASSERT_NCONTAINS(C, E) ELLE_ASSERT(true)
# define ELLE_ASSERT_NO_OTHER_EXCEPTION ELLE_ASSERT(true)
#endif

/// Provide a way for generating code only if evolving in the DEBUG mode.
#if defined(DEBUG) || !defined(NDEBUG)
# define ELLE_STATEMENT(...)                                           \
  __VA_ARGS__
#else
# define ELLE_STATEMENT(...)
#endif

namespace elle
{
  /** Throw an AssertError if the predicate is false.
   *
   *  @param predicate   Thruth value to test.
   *  @param message     Error message if the predicate is false.
   *  @param file        File the assertion is in.
   *  @param line        Line the assertion is at.
   *  @return            @param predicate if it is true.
   *  @throw AssertError if @param predicate is false.
   */
  template <typename T>
  ELLE_API
  T const&
  _elle_assert(T const& predicate,
               std::string const& message,
               char const* file,
               int line);
  template <typename T>
  ELLE_API
  T&
  _elle_assert(T& predicate,
               std::string const& message,
               char const* file,
               int line);

  // Generate a specialized assert function for operators.
#define ELLE_ASSERT_OP_CHECK(_op_, _abbr_)                              \
  template <typename A, typename B>                                     \
  void _assert_ ## _abbr_(A&& a,                                        \
                          B&& b,                                        \
                          char const* a_str,                            \
                          char const* b_str,                            \
                          char const* file,                             \
                          int line)                                     \
  {                                                                     \
    if (not (std::forward<A>(a) _op_ std::forward<B>(b)))               \
      _elle_assert(false,                                               \
              elle::sprintf("%s " #_op_ " %s is false: (%s=%s, %s=%s)", \
                            a_str, b_str, a_str, a, b_str, b),          \
              file,                                                     \
              line);                                                    \
  }
/**/

  // Dump assert functions for each operator.
  ELLE_ASSERT_OP_CHECK(==, eq);
  ELLE_ASSERT_OP_CHECK(!=, neq);
  ELLE_ASSERT_OP_CHECK(>, gt);
  ELLE_ASSERT_OP_CHECK(>=, gte);
  ELLE_ASSERT_OP_CHECK(<, lt);
  ELLE_ASSERT_OP_CHECK(<=, lte);
#undef ELLE_ASSERT_OP_CHECK

  template <typename C, typename E>
  void _assert_contains(C const& container,
                         E const& element,
                        char const* c_str,
                        char const* e_str,
                        char const* file,
                        int line)
  {
    if (container.find(element) == container.end())
      _elle_assert(
        false,
        elle::sprintf("%s does not contain %s: (%s=%s, %s=%s)",
                      c_str, e_str, c_str, container, e_str, element),
        file,
        line);
  }

  template <typename C, typename E>
  void _assert_ncontains(C const& container,
                         E const& element,
                         char const* c_str,
                         char const* e_str,
                         char const* file,
                         int line)
  {
    if (container.find(element) != container.end())
      _elle_assert(
        false,
        elle::sprintf("%s contains %s: (%s=%s, %s=%s)",
                      c_str, e_str, c_str, container, e_str, element),
        file,
        line);
  }
}

#include <elle/assert.hxx>
