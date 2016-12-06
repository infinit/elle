#pragma once

namespace elle
{
  template <typename ... Args>
  void
  print(std::ostream& o, std::string const& fmt, Args&& ... args);

  template <typename ... Args>
  std::string
  print(std::string const& fmt, Args&& ... args);
}

#include <elle/print.hxx>
