#pragma once

#include <string>
#include <type_traits>

#include <elle/err.hh>
#include <elle/meta.hh>
#include <elle/printf.hh>

// FIXME: C++17: migrate to std::string_view.  Explore std::from_chars
namespace elle
{
  /// Extract a value of type `Dest` from `s`.
  ///
  /// @throws std::out_of_range or std::invalid_argument.
  template <typename Dest>
  Dest
  from_string(std::string const& s);

  bool
  from_string(std::string const& s, bool*);

  // GCC 4.9 does not catch negative numbers as an error for unsigned
  // types.
  namespace
  {
    template <typename Type>
    auto
    check_non_negative(std::string const& s)
    {
      meta::static_if<std::is_unsigned<Type>{}>
        ([&s]
        {
          for (auto c: s)
            if (c == '-')
              err<std::out_of_range>("invalid positive number: %s", s);
            else if (!std::isspace(c))
              break;
        })();
    }
  }

  // Types that have their std::sto<TYPE> function.
#define ELLE_DEFINE(Type, Conversion)                           \
  inline                                                        \
  Type                                                          \
  from_string(std::string const& s, Type*)                      \
  {                                                             \
    check_non_negative<Type>(s);                                \
    auto size = size_t{};                                       \
    auto res = Conversion(s, &size);                            \
    if (size < s.size())                                        \
      err<std::invalid_argument>("invalid number: %s", s);      \
    return res;                                                 \
  }

  ELLE_DEFINE(double, std::stod);
  ELLE_DEFINE(float, std::stof);
  ELLE_DEFINE(int, std::stoi);
  ELLE_DEFINE(long double, std::stold);
  ELLE_DEFINE(long long, std::stoll);
  ELLE_DEFINE(long, std::stol);
  ELLE_DEFINE(unsigned long long, std::stoull);
  ELLE_DEFINE(unsigned long, std::stoul);
  ELLE_DEFINE(unsigned, std::stou);

#undef ELLE_DEFINE

  // Types that don't have a standard conversion routine.
#define ELLE_DEFINE(Type)                               \
  inline                                                \
  Type                                                  \
  from_string(std::string const& s, Type*)              \
  {                                                     \
    auto v = from_string<int>(s);                       \
    if (std::numeric_limits<Type>::min() <= v           \
        && v <= std::numeric_limits<Type>::max())       \
      return v;                                         \
    else                                                \
      err<std::out_of_range>("invalid number: %s", s);  \
  }

  ELLE_DEFINE(signed char);
  ELLE_DEFINE(signed short);
  ELLE_DEFINE(unsigned char);
  ELLE_DEFINE(unsigned short);

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
