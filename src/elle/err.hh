#pragma once

#include <utility>

#include <elle/compiler.hh>
#include <elle/print-fwd.hh>

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
    elle::err(print(fmt, std::forward<Args>(args)...));
  }

  /// Format and throw an exception of type \E.
  template <typename E, typename ... Args>
  ELLE_COMPILER_ATTRIBUTE_NORETURN
  void
  err(std::string const& fmt, Args&& ... args)
  {
    throw E(print(fmt, std::forward<Args>(args)...));
  }
}

#include <elle/print.hh>
