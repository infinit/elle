#ifndef ELLE_ASSERT_HH
# define ELLE_ASSERT_HH

# include <elle/compiler.hh>
# include <elle/printf.hh>
# include <elle/types.hh>

# include <string>
# include <stdexcept>

namespace elle
{
  class AssertError:
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

  void _assert(bool condition,
               std::string const& message,
               char const* file,
               int line);
}

/// Enforce a condition is true (always present in the code)
/// @see ELLE_ASSERT for debug only assertions.
#  define ELLE_ENFORCE(_condition_)                                           \
  ::elle::_assert(_condition_, #_condition_, __FILE__, __LINE__)
#  define ELLE_ENFORCE_EQ(A, B)                                               \
  ::elle::_assert(A == B,                                                     \
                  ::elle::sprintf(#A " == " #B " (%s != %s)", A, B),          \
                  __FILE__, __LINE__)
#  define ELLE_ENFORCE_NEQ(A, B)                                              \
  ::elle::_assert(A != B,                                                     \
                  ::elle::sprintf(#A " != " #B " (%s == %s)", A, B),          \
                  __FILE__, __LINE__)
#  define ELLE_ENFORCE_GT(A, B)                                               \
  ::elle::_assert(A > B,                                                      \
                  ::elle::sprintf(#A " > " #B " (%s <= %s)", A, B),           \
                  __FILE__, __LINE__)
#  define ELLE_ENFORCE_GTE(A, B)                                              \
  ::elle::_assert(A >= B,                                                     \
                  ::elle::sprintf(#A " >= " #B " (%s < %s)", A, B),           \
                  __FILE__, __LINE__)
#  define ELLE_ENFORCE_LT(A, B)                                               \
  ::elle::_assert(A < B,                                                      \
                  ::elle::sprintf(#A " < " #B " (%s >= %s)", A, B),           \
                  __FILE__, __LINE__)
#  define ELLE_ENFORCE_LTE(A, B)                                              \
  ::elle::_assert(A <= B,                                                     \
                  ::elle::sprintf(#A " <= " #B " (%s > %s)", A, B),           \
                  __FILE__, __LINE__)

# if defined(DEBUG) || !defined(NDEBUG)
/// Throw if the condition is unmet.
#  define ELLE_ASSERT(_condition_) ELLE_ENFORCE(_condition_)
#  define ELLE_ASSERT_EQ(A, B) ELLE_ENFORCE_EQ(A, B)
#  define ELLE_ASSERT_NEQ(A, B) ELLE_ENFORCE_NEQ(A, B)
#  define ELLE_ASSERT_GT(A, B) ELLE_ENFORCE_GT(A, B)
#  define ELLE_ASSERT_GTE(A, B) ELLE_ENFORCE_GTE(A, B)
#  define ELLE_ASSERT_LT(A, B) ELLE_ENFORCE_LT(A, B)
#  define ELLE_ASSERT_LTE(A, B) ELLE_ENFORCE_LTE(A, B)
# else
#  define ELLE_ASSERT(_condition_) ((void) 0)
#  define ELLE_ASSERT_EQ(A, B) ELLE_ASSERT(true)
#  define ELLE_ASSERT_NEQ(A, B) ELLE_ASSERT(true)
#  define ELLE_ASSERT_GT(A, B) ELLE_ASSERT(true)
#  define ELLE_ASSERT_GTE(A, B) ELLE_ASSERT(true)
#  define ELLE_ASSERT_LT(A, B) ELLE_ASSERT(true)
#  define ELLE_ASSERT_LTE(A, B) ELLE_ASSERT(true)
# endif

/// Provide a way for generating code only if evolving in the DEBUG mode.
# if defined(DEBUG) || !defined(NDEBUG)
#  define ELLE_STATEMENT(...)                                           \
  __VA_ARGS__
# else
#  define ELLE_STATEMENT(...)
# endif

namespace elle
{
  /// Print the message and abort program execution.
  ELLE_COMPILER_ATTRIBUTE_NORETURN
  void
  abort(std::string const& msg);

  /// Abort the program. Flags unreachable code.
  ELLE_COMPILER_ATTRIBUTE_NORETURN
  void
  unreachable();
}

#endif
