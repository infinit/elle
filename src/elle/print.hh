#pragma once

#include <iosfwd>

namespace elle
{
  /// Print a formatted string to a given stream.
  ///
  /// @param o The output stream.
  /// @param fmt The un-formatted string specifying how to format and interpret
  ///            the given data.
  /// @param args The arguments specifying data to print.
  template <typename ... Args>
  void
  print(std::ostream& o, std::string const& fmt, Args&& ... args);

  /// Return a formatted string.
  ///
  /// @param fmt The un-formatted string specifying how to format and interpret
  ///            the given data.
  /// @param args The arguments specifying data to print.
  /// @returns The string, formatted.
  template <typename ... Args>
  std::string
  print(std::string const& fmt, Args&& ... args);
}

#include <elle/print.hxx>
