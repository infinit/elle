#ifndef INFINIT_REACTOR_FWD_HH
# define INFINIT_REACTOR_FWD_HH

# include <vector>

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
  template <typename R = void>
  class VThread;
  class Waitable;

  typedef std::vector<Signal*> Signals;
  typedef std::vector<Waitable*> Waitables;
}

#endif
