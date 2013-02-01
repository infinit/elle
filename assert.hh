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

}

# if defined(DEBUG) || !defined(NDEBUG)
/// Throw if the condition is unmet.
# define ELLE_ASSERT(_condition_)                                             \
  do                                                                          \
    {                                                                         \
      if (!(_condition_))                                                     \
        throw elle::AssertError(#_condition_, __FILE__, __LINE__);            \
    } while (0)                                                               \
/**/


# else
#  define ELLE_ASSERT(_condition_) ((void) 0)
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
