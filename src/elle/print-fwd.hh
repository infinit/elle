#pragma once

#include <iosfwd>
#include <string>

namespace elle
{
  /// Print a formatted string to a given stream.
  ///
  /// @param o The output stream.
  /// @param fmt The un-formatted string specifying how to format and interpret
  ///            the given data.
  /// @param args The arguments specifying data to print.
  ///
  /// @return o
  ///
  /// Examples of formats:
  /// - "{0}, {1}, {2}": prints arguments 1, 2, and 3.
  /// - "{2}, {1}, {2}": prints arguments 3, 2, and 3.
  /// - "{1?one}": prints argument "one" if arg 1 evaluates to true.
  /// - "{1?({1})}": prints arg 2 in parens if it evaluates to true.
  template <typename ... Args>
  std::ostream&
  print(std::ostream& o, std::string const& fmt, Args&& ... args);

  /// A formatted string.
  ///
  /// @param fmt The un-formatted string specifying how to format and interpret
  ///            the given data.
  /// @param args The arguments specifying data to print.
  /// @returns The string, formatted.
  template <typename ... Args>
  std::string
  print(std::string const& fmt, Args&& ... args);

  /// Whether a stream is set for debugging output.
  ///
  /// Armed with `%r` in print's format.
  bool
  repr(std::ostream const& o);

  /// Set whether a stream is set for debugging output.
  void
  repr(std::ostream& o, bool debugging);

  template <typename T>
  std::string
  to_string(T&& o);
}
