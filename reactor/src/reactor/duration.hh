#ifndef REACTOR_DURATION_HH
# define REACTOR_DURATION_HH

# include <boost/date_time/posix_time/posix_time_types.hpp>
# include <boost/optional.hpp>

namespace reactor
{
  typedef boost::posix_time::time_duration Duration;
  typedef boost::optional<Duration> DurationOpt;

  std::ostream&
  operator << (std::ostream& s, const DurationOpt& opt);

  boost::posix_time::millisec
  operator "" _ms(unsigned long long time);

  boost::posix_time::seconds
  operator "" _sec(unsigned long long time);

  boost::posix_time::minutes
  operator "" _min(unsigned long long time);

  boost::posix_time::hours
  operator "" _h(unsigned long long time);
}

#endif
