#include <elle/Duration.hh>

namespace elle
{
  std::ostream&
  operator << (std::ostream& s, const DurationOpt& opt)
  {
    if (opt)
      return s << opt.get();
    else
      return s << "no duration";
  }
}

boost::posix_time::millisec
operator "" _ms(unsigned long long time)
{
  return boost::posix_time::millisec{static_cast<long>(time)};
}

boost::posix_time::seconds
operator "" _sec(unsigned long long time)
{
  return boost::posix_time::seconds{static_cast<long>(time)};
}

boost::posix_time::minutes
operator "" _min(unsigned long long time)
{
  return boost::posix_time::minutes{static_cast<long>(time)};
}

boost::posix_time::hours
operator "" _h(unsigned long long time)
{
  return boost::posix_time::hours{static_cast<long>(time)};
}
