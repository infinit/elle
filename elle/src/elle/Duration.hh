#ifndef ELLE_DURATION_HH
# define ELLE_DURATION_HH

# include <boost/optional.hpp>

# include <elle/compiler.hh>
# include <elle/time.hh>

namespace elle
{
  typedef boost::posix_time::time_duration Duration;
  typedef boost::optional<Duration> DurationOpt;

  ELLE_API
  std::ostream&
  operator << (std::ostream& s, const DurationOpt& opt);
}

ELLE_API
boost::posix_time::millisec
operator "" _ms(unsigned long long time);

ELLE_API
boost::posix_time::seconds
operator "" _sec(unsigned long long time);

ELLE_API
boost::posix_time::minutes
operator "" _min(unsigned long long time);

ELLE_API
boost::posix_time::hours
operator "" _h(unsigned long long time);

#endif
