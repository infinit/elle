#include <iostream>
#include <reactor/exception.hh>
#include <reactor/scheduler.hh>
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

  std::ostream&
  operator << (std::ostream& s, Exception const& e)
  {
    s << e.what();
    return s;
  }

  Backtrace const&
  Exception::backtrace() const
  {
    return _backtrace;
  }

  Terminate::Terminate(Scheduler& scheduler)
    : Super(scheduler, "thread termination")
  {}
}
