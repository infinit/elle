#include <elle/from-string.hh>

#include <stdexcept>
#include <type_traits>
#include <unordered_map>

#include <boost/algorithm/string/case_conv.hpp>

#include <elle/err.hh>
#include <elle/find.hh>
#include <elle/meta.hh>
#include <elle/printf.hh> // for elle::err

namespace elle
{
  // GCC 4.9 does not catch negative numbers as an error for unsigned
  // types.
  namespace
  {
    template <typename Type>
    auto
    check_unsigned(std::string const& s)
    {
      meta::static_if<std::is_unsigned<Type>{}>
        ([&s]
        {
          for (auto c: s)
            if (c == '-')
              err<std::out_of_range>("invalid unsigned number: %s", s);
            else if (!std::isspace(c))
              break;
        })();
    }
  }

  // Types that have their std::sto<TYPE> function.
#define ELLE_DEFINE(Type, Conversion)                           \
  Type                                                          \
  from_string(std::string const& s, Type*)                      \
  {                                                             \
    check_unsigned<Type>(s);                                    \
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

  bool
  from_string(std::string const& s, bool*)
  {
    static auto const map = std::unordered_map<std::string, bool>
      {
        {"0",     false},
        {"1",     true},
        {"false", false},
        {"n",     false},
        {"no",    false},
        {"true",  true},
        {"y",     true},
        {"yes",   true},
      };
    if (auto it = elle::find(map, boost::to_lower_copy(s)))
      return it->second;
    else
      elle::err<std::invalid_argument>("invalid boolean value: %s", s);
  }
}
