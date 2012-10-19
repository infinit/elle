#ifndef ELLE_ASSERT_HH
# define ELLE_ASSERT_HH

# include <string>

# include <elle/compiler.hh>

namespace elle
{
  /// Print the message and abort program execution.
  ELLE_COMPILER_ATTRIBUTE_NORETURN
  void
  abort(const std::string& msg);

  /// Abort the program. Flags unreachable code.
  ELLE_COMPILER_ATTRIBUTE_NORETURN
  void
  unreachable();
}

#endif
