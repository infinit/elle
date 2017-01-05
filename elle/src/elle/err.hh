#pragma once

#include <utility>

#include <elle/compiler.hh>
#include <elle/printf-fwd.hh>

namespace elle
{
  /// Throw an elle::Error.
  ELLE_API
  ELLE_COMPILER_ATTRIBUTE_NORETURN
  void
  err(std::string const& msg);

  /// Format and throw an elle::Error.
  template <typename ... Args>
  ELLE_COMPILER_ATTRIBUTE_NORETURN
  void
  err(std::string const& fmt, Args&& ... args)
  {
    elle::err(sprintf(fmt, std::forward<Args>(args)...));
  }

  template <typename E, typename ... Args>
  ELLE_COMPILER_ATTRIBUTE_NORETURN
  void
  err(std::string const& fmt, Args&& ... args)
  {
    throw E(sprintf(fmt, std::forward<Args>(args)...));
  }
}

