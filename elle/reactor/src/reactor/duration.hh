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
}

#endif
