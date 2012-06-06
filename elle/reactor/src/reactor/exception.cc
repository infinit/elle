#include <iostream>
#include <reactor/exception.hh>
#include <reactor/thread.hh>

namespace reactor
{
  Exception::Exception(Scheduler& scheduler, const std::string& message)
    : std::runtime_error(message)
    , _scheduler(scheduler)
    , _backtrace(Backtrace::current())
  {
    if (Thread* t = scheduler.current())
      _backtrace.strip_base(t->_backtrace_root);
  }

  Exception::~Exception() throw ()
  {}

  Backtrace const&
  Exception::backtrace() const
  {
    return _backtrace;
  }

  Terminate::Terminate(Scheduler& scheduler)
    : Super(scheduler, "thread termination")
  {}
}
