#pragma once

#include <string>

// FIXME: C++17: migrate to std::string_view.
namespace elle
{
  bool
  from_string(std::string const& s, bool*);

#define ELLE_DEFINE(Type, Conversion)           \
  inline                                        \
  Type                                          \
  from_string(std::string const& s, Type*)      \
  {                                             \
    return Conversion(s);                       \
  }

  ELLE_DEFINE(int, std::stoi);
  ELLE_DEFINE(unsigned, std::stou);

#undef ELLE_DEFINE

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

  template <typename Dest>
  auto
  from_string(char const* s)
    -> std::enable_if_t<std::is_same<Dest, char const*>{},
                        char const*>
  {
    return s;
  }
}
