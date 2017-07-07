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
  convert_capacity(long double value, std::string const& unit)
  {
    if (unit == "B" || unit == "")
      return value;
    else if (unit == "kB")
      return value * 1000;
    else if (unit == "KiB")
      return value * 1024;
    else if (unit == "MB")
      return value * 1000 * 1000;
    else if (unit == "MiB")
      return value * 1024 * 1024;
    else if (unit == "GB")
      return value * 1000 * 1000 * 1000;
    else if (unit == "GiB")
      return value * 1024 * 1024 * 1024;
    else if (unit == "TB")
      return value * 1000 * 1000 * 1000 * 1000;
    else if (unit == "TiB")
      return value * 1024 * 1024 * 1024 * 1024;
    else
      elle::err("invalid capacity: %s", unit);
  }

  int64_t
  convert_capacity(std::string value)
  {
    std::string const unit = [&]
      {
        auto const to_find = std::vector<std::string>
        {
          // "B" MUST be the last element.
          "kB", "MB", "GB", "TB", "KiB", "MiB", "GiB", "TiB", "B"
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
