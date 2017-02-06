#include <elle/bytes.hh>

#include <cmath>

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
      {"B", "kiB", "MiB", "GiB", "TiB", "EiB", "ZiB"};
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
}
