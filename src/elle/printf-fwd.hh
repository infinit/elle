#pragma once

#include <iosfwd>

namespace elle
{
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
