#include <iostream>
#include <reactor/exception.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

namespace reactor
{
  Exception::Exception(const std::string& message)
    : Exception(elle::Backtrace(), message)
  {}

  Exception::Exception(elle::Backtrace const& bt, const std::string& message)
    : elle::Exception(bt, message)
  {
    // if (Scheduler* sched = reactor::Scheduler::scheduler())
    //   if (Thread* t = sched->current())
    //     _backtrace.strip_base(t->_backtrace_root);
  }

  Exception::~Exception() throw ()
  {}

  Terminate::Terminate()
    : Super("thread termination")
  {}
}
