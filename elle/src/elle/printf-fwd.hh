#pragma once

#include <iosfwd>

namespace elle
{
  /// Print on std::cout.
  /// \return std::cout.
  template <typename F, typename... T>
  std::ostream&
  printf(F&& fmt,
         T&&... values);

  /// Print on stream.
  template <typename F, typename... T>
  std::ostream&
  fprintf(std::ostream& out,
          F&& fmt,
          T&&... values);

  /// Print in string.
  template <typename F, typename... T>
  std::string
  sprintf(F&& fmt,
          T&&... values);
}
