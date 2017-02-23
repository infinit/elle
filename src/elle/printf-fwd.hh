#pragma once

#include <iosfwd>

#include <boost/format/exceptions.hpp>

#include <elle/compiler.hh>

namespace elle
{
  /// Raise an elle::Exception about a Boost.Format failure.
  ELLE_COMPILER_ATTRIBUTE_NORETURN
  void
  format_error(std::string const& fmt, boost::io::format_error const& e);

  namespace
  {
    /// Print on stream.
    template <typename F, typename... T>
    std::ostream&
    fprintf(std::ostream& out, F&& fmt, T&&... values);

    /// Print in string.
    template <typename F, typename... T>
    std::string
    sprintf(F&& fmt, T&&... values);
  }
}
