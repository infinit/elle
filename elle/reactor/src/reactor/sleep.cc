#include <reactor/exception.hh>
#include <reactor/scheduler.hh>
#include <reactor/sleep.hh>

namespace reactor
{
  Sleep::Sleep(Scheduler& scheduler, Duration d)
    : Operation(scheduler)
    , _duration(d)
    , _timer(scheduler.io_service(), d)
  {}

  void
  Sleep::_abort()
  {
    _timer.cancel();
    _signal();
  }

  void
  Sleep::_wakeup(const boost::system::error_code& error)
  {
    if (error == boost::system::errc::operation_canceled)
      return;
    if (error)
      _raise(new Exception(scheduler(), error.message()));
    _signal();
  }

  void
  Sleep::_start()
  {
    _timer.async_wait(boost::bind(&Sleep::_wakeup, this, _1));
  }
}
