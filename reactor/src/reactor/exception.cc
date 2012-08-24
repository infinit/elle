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
    , _inner(0)
  {
    if (Thread* t = scheduler.current())
      _backtrace.strip_base(t->_backtrace_root);
  }

  Exception::Exception(Scheduler& scheduler, const std::string& message,
                       Backtrace const& bt)
    : std::runtime_error(message)
    , _scheduler(scheduler)
    , _backtrace(bt)
    , _inner(0)
  {
    if (Thread* t = scheduler.current())
      _backtrace.strip_base(t->_backtrace_root);
  }

  Exception::~Exception() throw ()
  {}

  Exception const*
  Exception::inner_exception() const
  {
    return this->_inner;
  }

  void
  Exception::inner_exception(Exception* e)
  {
    this->_inner = e;
  }

  std::ostream&
  operator << (std::ostream& s, Exception const& e)
  {
    s << e.what() << std::endl;
    s << e.backtrace();
    if (Exception const* inner = e.inner_exception())
      s << std::endl << "Exception was triggerd by:" << *inner;
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
