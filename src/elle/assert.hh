#pragma once

#include <string>
#include <stdexcept>

#include <elle/compiler.hh>
#include <elle/fwd.hh>
#include <elle/print.hh>
#include <elle/unreachable.hh>

namespace elle ELLE_API
{
  /// Exception thrown when an assertion failed.
  ///
  /// @note You should never catch directly `AssertError`, nor its base class
  /// `std::exception`, except in the main function of the program.
  class AssertError
    : public std::exception
  {
  public:
    AssertError(char const* condition,
                char const* file, int line) noexcept;
    AssertError() = default;
    AssertError(AssertError const& that) = default;
    AssertError& operator=(AssertError const& that) = default;
    ~AssertError() = default;

    /// The error message, including file and line.
    const char*
    what() const noexcept override;
    /// The stack of frames leading to the assertion.
    std::shared_ptr<Backtrace const>
    backtrace() const noexcept;

  private:
    /// The message, including file and line.
    std::string _what = "";
    std::shared_ptr<Backtrace const> _bt = nullptr;
  };

  /// Print the message and abort program execution.
  ///
  /// @throws AssertError, unless $ENV{"ELLE_REAL_ASSERT"} is true,
  ///    in which case std::abort is called.
  ELLE_COMPILER_ATTRIBUTE_NORETURN
  void
  _abort(std::string const& msg, char const* file, int line);

  /// An AssertError handler.
  using AbortCallback
    = std::function<auto (AssertError const& except) -> void>;

  /// Register an action to perform before raising/aborting.
  template <typename Fun>
  auto on_abort(Fun&& fun)
    -> decltype(AbortCallback(fun), void());
}


#define ELLE_ABORT(...)                                         \
  ::elle::_abort(elle::print(__VA_ARGS__), __FILE__, __LINE__)

/// Enforce a condition is true (always present in the code)
/// @see ELLE_ASSERT for debug only assertions.
#define ELLE_ENFORCE(_condition_)                                       \
  ::elle::_elle_assert(_condition_, #_condition_, __FILE__, __LINE__)

#define ELLE_ENFORCE_OP(Op, A, B)                               \
  ::elle::_assert_ ## Op(A, B, #A, #B, __FILE__, __LINE__)

#define ELLE_ENFORCE_EQ(A, B)  ELLE_ENFORCE_OP(eq,  A, B)
#define ELLE_ENFORCE_NEQ(A, B) ELLE_ENFORCE_OP(neq, A, B)
#define ELLE_ENFORCE_GT(A, B)  ELLE_ENFORCE_OP(gt,  A, B)
#define ELLE_ENFORCE_GTE(A, B) ELLE_ENFORCE_OP(gte, A, B)
#define ELLE_ENFORCE_LT(A, B)  ELLE_ENFORCE_OP(lt,  A, B)
#define ELLE_ENFORCE_LTE(A, B) ELLE_ENFORCE_OP(lte, A, B)

#define ELLE_ENFORCE_CONTAINS(C, E)   ELLE_ENFORCE_OP(contains,  C, E)
#define ELLE_ENFORCE_NCONTAINS(C, E)  ELLE_ENFORCE_OP(ncontains, C, E)

/// Use after the last catch close to enforce no other exception is caught
#define ELLE_ENFORCE_NO_OTHER_EXCEPTION                                 \
  catch(...)                                                            \
  {                                                                     \
    ::elle::_elle_assert(                                               \
      false, elle::print("Expected no more exception, got '%s'",        \
                         elle::exception_string()),                     \
      __FILE__, __LINE__);                                              \
  }

#if defined DEBUG || !defined NDEBUG
/// Throw if the condition is unmet.
# define ELLE_ASSERT           ELLE_ENFORCE
# define ELLE_ASSERT_EQ        ELLE_ENFORCE_EQ
# define ELLE_ASSERT_NEQ       ELLE_ENFORCE_NEQ
# define ELLE_ASSERT_GT        ELLE_ENFORCE_GT
# define ELLE_ASSERT_GTE       ELLE_ENFORCE_GTE
# define ELLE_ASSERT_LT        ELLE_ENFORCE_LT
# define ELLE_ASSERT_LTE       ELLE_ENFORCE_LTE
# define ELLE_ASSERT_CONTAINS  ELLE_ENFORCE_CONTAINS
# define ELLE_ASSERT_NCONTAINS ELLE_ENFORCE_NCONTAINS
# define ELLE_ASSERT_NO_OTHER_EXCEPTION ELLE_ENFORCE_NO_OTHER_EXCEPTION
#else
# define ELLE_ASSERT(_)              ((void) 0)
# define ELLE_ASSERT_EQ(A, B)        ELLE_ASSERT(true)
# define ELLE_ASSERT_NEQ(A, B)       ELLE_ASSERT(true)
# define ELLE_ASSERT_GT(A, B)        ELLE_ASSERT(true)
# define ELLE_ASSERT_GTE(A, B)       ELLE_ASSERT(true)
# define ELLE_ASSERT_LT(A, B)        ELLE_ASSERT(true)
# define ELLE_ASSERT_LTE(A, B)       ELLE_ASSERT(true)
# define ELLE_ASSERT_CONTAINS(C, E)  ELLE_ASSERT(true)
# define ELLE_ASSERT_NCONTAINS(C, E) ELLE_ASSERT(true)
# define ELLE_ASSERT_NO_OTHER_EXCEPTION ELLE_ASSERT(true)
#endif

/// Provide a way for generating code only if evolving in the DEBUG mode.
#if defined(DEBUG) || !defined(NDEBUG)
# define ELLE_STATEMENT(...)                    \
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
  decltype(auto)
  _elle_assert(T&& predicate,
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
      _elle_assert(                                                     \
        false,                                                          \
        elle::print("%s " #_op_ " %s is false: (%s=%s, %s=%s)",         \
                    a_str, b_str, a_str, a, b_str, b),                  \
        file,                                                           \
        line);                                                          \
  }

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
        elle::print("%s does not contain %s: (%s=%s, %s=%s)",
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
        elle::print("%s contains %s: (%s=%s, %s=%s)",
                    c_str, e_str, c_str, container, e_str, element),
        file,
        line);
  }
}

#include <elle/assert.hxx>
