#ifndef ELLE_ASSERT_HH
# define ELLE_ASSERT_HH

# include <string>

# include <elle/compiler.hh>
# include <elle/types.hh>

/// Throw if the condition is unmet.
# define ELLE_ASSERT(_condition_)                                       \
  do                                                                    \
    {                                                                   \
      if (!(_condition_))                                               \
        throw elle::Exception("assertion failed (%s) at %s:%s",         \
                              #_condition_, __FILE__, __LINE__);        \
    } while (0)

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
