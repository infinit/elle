#pragma once

#include <iosfwd>
#include <string>

#include <boost/format/exceptions.hpp>

#include <elle/compiler.hh>
#include <elle/printf-fwd.hh>

namespace elle
{
  /// Raise an elle::Exception about a Boost.Format failure.
  ELLE_COMPILER_ATTRIBUTE_NORETURN
  void
  format_error(std::string const& fmt, boost::io::format_error const& e);
}

#include <elle/printf.hxx>
