#ifndef INFINIT_REACTOR_FWD_HH
# define INFINIT_REACTOR_FWD_HH

# include <vector>

# include <boost/date_time/posix_time/posix_time.hpp>
# include <boost/optional.hpp>

# ifdef BUILDING_REACTOR_DLL
#  define REACTOR_API __declspec(dllexport)
# else
#  define REACTOR_API __declspec(dllimport)
# endif

namespace reactor
{
  class Exception;
  class Operation;
  class Scheduler;
  class Signal;
  class Sleep;
  class Thread;
  class Waitable;

  typedef boost::posix_time::time_duration Duration;
  typedef boost::optional<Duration> DurationOpt;
  typedef std::vector<Signal*> Signals;
  typedef std::vector<Waitable*> Waitables;
}

#endif
