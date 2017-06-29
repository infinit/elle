#include <elle/time.hh>

#include <elle/printf.hh>

namespace elle
{
  boost::posix_time::ptime
  to_posix_time(std::string const& str)
  {
    boost::posix_time::ptime res;
    // Use the ISO extended input facet to interpret the string.
    std::stringstream ss(str);
    auto input_facet =
      std::make_unique<boost::posix_time::time_input_facet>();
    // ISO 8601
    input_facet->format("%Y-%m-%dT%H:%M:%S%F");
    ss.imbue(std::locale(ss.getloc(), input_facet.release()));
    if (!(ss >> res))
      err("invalid ISO8601 date: %s", str);
    // Check there isn't any leftover.
    std::string leftover;
    std::getline(ss, leftover);
    if (leftover.empty())
      return res;
    // Boost can't parse timezones, handle it manually.
    if (leftover == "Z")
      ; // Accept UTC suffix.
    else if ((leftover[0] == '+' || leftover[0] == '-') && leftover.size() == 5)
    {
      // Handle timezone.
      std::stringstream tz(leftover);
      int direction = tz.get() == '+' ? -1 : 1;
      int amount;
      tz >> amount;
      if (tz.get() != -1)
        err("garbage at end of date: %s", leftover);
      res += boost::posix_time::hours(direction * amount / 100);
    }
    else
      err("garbage at end of date: %s", leftover);
    if (!ss.eof())
    {
      std::string leftover;
      std::getline(ss, leftover);
      err("garbage at end of date: %s", leftover);
    }
    return res;
  }
}
