#include <boost/lexical_cast.hpp>

#include <reactor/exception.hh>
#include <reactor/operation.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

namespace reactor
{
  Operation::Operation(Scheduler& sched)
    : Super()
    , _sched(sched)
  {}

  bool
  Operation::run(DurationOpt timeout)
  {
    Thread* current = _sched.current();
    assert(current);
    start();
    try
      {
        if (!current->wait(*this, timeout))
          {
            abort();
            return false;
          }
      }
    catch (const Terminate&)
      {
        abort();
        throw;
      }
    return true;
  }

  void
  Operation::abort()
  {
    _abort();
  }

  void
  Operation::start()
  {
    _start();
  }

  Scheduler&
  Operation::scheduler()
  {
    return _sched;
  }
}
