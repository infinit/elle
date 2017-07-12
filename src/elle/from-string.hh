#pragma once

#include <string>

// FIXME: C++17: migrate to std::string_view.  Explore std::from_chars
namespace elle
{
  /// Extract a value of type `Dest` from `s`.
  ///
  /// @throws std::out_of_range or std::invalid_argument.
  template <typename Dest>
  Dest
  from_string(std::string const& s);

#define ELLE_DECLARE(Type)                      \
  Type                                          \
  from_string(std::string const& s, Type*)

  ELLE_DECLARE(bool);

  ELLE_DECLARE(double);
  ELLE_DECLARE(float);
  ELLE_DECLARE(int);
  ELLE_DECLARE(long double);
  ELLE_DECLARE(long long);
  ELLE_DECLARE(long);
  ELLE_DECLARE(signed char);
  ELLE_DECLARE(signed short);
  ELLE_DECLARE(unsigned char);
  ELLE_DECLARE(unsigned long long);
  ELLE_DECLARE(unsigned long);
  ELLE_DECLARE(unsigned short);
  ELLE_DECLARE(unsigned);

#undef ELLE_DECLARE

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
