#pragma once

#include <iosfwd>

namespace elle
{
  template <typename F, typename... T>
  void
  printf(F&& fmt,
         T&&... values);

  template <typename F, typename... T>
  std::ostream&
  fprintf(std::ostream& out,
          F&& fmt,
          T&&... values);

  template <typename F, typename... T>
  std::string
  sprintf(F&& fmt,
          T&&... values);
}

