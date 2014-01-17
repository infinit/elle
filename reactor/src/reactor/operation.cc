#include <boost/lexical_cast.hpp>

#include <reactor/exception.hh>
#include <reactor/operation.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

#include <elle/assert.hh>

namespace reactor
{
  Operation::Operation():
    Operation(*Scheduler::scheduler())
  {}

  Operation::Operation(Scheduler& sched):
    Super(),
    _running(false),
    _sched(sched)
  {}

  bool
  Operation::run(DurationOpt timeout)
  {
    Thread* current = _sched.current();
    ELLE_ASSERT(current);
    this->start();
    try
    {
      if (!current->wait(*this, timeout))
      {
        this->abort();
        return false;
      }
    }
    catch (const Terminate&)
    {
      this->abort();
      throw;
    }
    return true;
  }

  void
  Operation::abort()
  {
    if (this->running())
      this->_abort();
  }

  void
  Operation::start()
  {
    this->_running = true;
    this->_start();
  }

  void
  Operation::done()
  {
    this->_running = false;
    this->_signal();
  }

  Scheduler&
  Operation::scheduler()
  {
    return this->_sched;
  }
}
