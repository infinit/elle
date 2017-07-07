#include <elle/bytes.hh>

#include <cmath>

#include <boost/algorithm/string/case_conv.hpp>

#include <elle/err.hh>
#include <elle/printf.hh>
#include <elle/unreachable.hh>

namespace elle
{
  std::string
  human_data_size(double bytes, bool si)
  {
    // FIXME: use Boost instead.
    // http://www.boost.org/doc/libs/1_62_0/doc/html/boost_units/Examples.html#boost_units.Examples.autoscale
    constexpr const char* engineer[] =
      {"B", "kB", "MB", "GB", "TB", "EB", "ZB"};
    constexpr const char* computer[] =
      {"B", "KiB", "MiB", "GiB", "TiB", "EiB", "ZiB"};
    static_assert(sizeof engineer == sizeof computer,
                  "different scales");
    constexpr auto size = sizeof engineer / sizeof(*engineer);

    auto scale = si ? 1000. : 1024.;
    auto const& unit = si ? engineer : computer;
    for (uint64_t i = 0; i < size; ++i)
      if (i == size - 1 || bytes / std::pow(scale, i+1) < 1)
        return elle::sprintf("%.1f %s", bytes / pow(scale, i), unit[i]);
    elle::unreachable();
  }

  long double
  convert_capacity(long double value, std::string unit)
  {
    boost::to_lower(unit);
    if (unit == "b" || unit == "")
      return value;
    else if (unit == "kb")
      return value * 1000;
    else if (unit == "kib")
      return value * 1024;
    else if (unit == "mb")
      return value * 1000 * 1000;
    else if (unit == "mib")
      return value * 1024 * 1024;
    else if (unit == "gb")
      return value * 1000 * 1000 * 1000;
    else if (unit == "gib")
      return value * 1024 * 1024 * 1024;
    else if (unit == "tb")
      return value * 1000 * 1000 * 1000 * 1000;
    else if (unit == "tib")
      return value * 1024 * 1024 * 1024 * 1024;
    else
      elle::err("invalid capacity unit: %s", unit);
  }

  int64_t
  convert_capacity(std::string value)
  {
    boost::to_lower(value);
    std::string const unit = [&]
      {
        auto const to_find = std::vector<std::string>
        {
          // "b" MUST be the last element.
          "kb", "mb", "gb", "tb", "kib", "mib", "gib", "tib", "b"
        };
        const char* res = nullptr;
        for (auto const& t: to_find)
          if (res = std::strstr(value.c_str(), t.c_str()))
            break;
        return res ? res : "";
      }();
    auto const double_value =
      std::stold(value.substr(0, value.size() - unit.size()));
    return std::llround(convert_capacity(double_value, unit));
  }

  boost::optional<int64_t>
  convert_capacity(boost::optional<std::string> capacity)
  {
    return capacity
      ? convert_capacity(*capacity)
      : boost::optional<int64_t>{};
  }
}
