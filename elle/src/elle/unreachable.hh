#pragma once

#include <string>

#include <elle/compiler.hh>

namespace elle ELLE_API
{
  /// Abort the program. Flags unreachable code.
  ELLE_COMPILER_ATTRIBUTE_NORETURN
  void
  unreachable();
}
