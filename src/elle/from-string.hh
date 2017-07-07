#pragma once

#include <string>

// FIXME: C++17: migrate to std::string_view.
namespace elle
{
  bool
  from_string(std::string const& s, bool*);

  inline
  std::string
  from_string(std::string const& s, std::string*)
  {
    return s;
  }

  template <typename Dest>
  Dest
  from_string(std::string const& s)
  {
    return from_string(s, static_cast<Dest*>(nullptr));
  }
}
