#include <reactor/TimeoutGuard.hh>

#include <reactor/scheduler.hh>
#include <reactor/exception.hh>

namespace reactor
{
  ELLE_LOG_COMPONENT("reactor.Timeout");

  TimeoutGuard::TimeoutGuard(reactor::Duration delay)
    : _delay(delay)
    , _timer(reactor::scheduler().io_service(), delay)
  {
    ELLE_TRACE_SCOPE("%s: start", *this);
    auto current = reactor::scheduler().current();
    auto timeout_msg = elle::sprintf("%s: time out", *this);
    this->_timer.async_wait(
      [delay, current, timeout_msg]
      (boost::system::error_code const& e)
      {
        if (e == boost::system::errc::operation_canceled)
          return;
        if (e)
          ELLE_ABORT("unexpected timer error: %s", e);
        ELLE_TRACE_SCOPE("%s", timeout_msg);
        current->raise<reactor::Timeout>(delay);
      });
  }

  TimeoutGuard::~TimeoutGuard()
  {
    ELLE_TRACE_SCOPE("%s: cancel", *this);
    this->_timer.cancel();
  }
}
