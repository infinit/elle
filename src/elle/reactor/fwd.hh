#pragma once

#include <vector>

#ifdef BUILDING_REACTOR_DLL
# define REACTOR_API __declspec(dllexport)
#else
# define REACTOR_API __declspec(dllimport)
#endif

namespace elle
{
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

    typedef std::vector<Signal*> Signals;
    typedef std::vector<Waitable*> Waitables;
  }
}
