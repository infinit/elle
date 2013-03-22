#ifndef ELLE_ASSERT_HH
# define ELLE_ASSERT_HH

# include <elle/compiler.hh>
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

# if defined(DEBUG) || !defined(NDEBUG)

/// Throw if the condition is unmet.
# define ELLE_ASSERT(_condition_)                                       \
  ::elle::_assert(_condition_, #_condition_, __FILE__, __LINE__)        \

# define ELLE_ASSERT_EQ(A, B)                                           \
  ::elle::_assert(A == B,                                               \
                    ::elle::sprintf(#A " == " #B " (%s != %s)", A, B),  \
                    __FILE__, __LINE__)                                 \

# else
#  define ELLE_ASSERT(_condition_) ((void) 0)
#  define ELLE_ASSERT_EQ(A, B) ELLE_ASSERT()
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
