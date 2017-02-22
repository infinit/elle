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
  class Barrier;
  class Mutex;
  class Operation;
  class Scheduler;
  class Semaphore;
  class Signal;
  class Sleep;
  class Thread;
  class TimeoutGuard;
  template <typename R = void>
  class VThread;
  class Waitable;

  using Signals = std::vector<Signal *>;
  using Waitables = std::vector<Waitable *>;
}

#endif
