#include <elle/reactor/TimeoutGuard.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/reactor/exception.hh>

namespace elle
{
  namespace reactor
  {
    ELLE_LOG_COMPONENT("elle.reactor.Timeout");

    TimeoutGuard::TimeoutGuard(reactor::Duration delay)
      : _delay(delay)
      , _timer(reactor::scheduler().io_service(), delay)
    {
      ELLE_TRACE_SCOPE("%s: start", *this);
      auto current = reactor::scheduler().current();
      auto timeout_msg = elle::sprintf("%s: timeout %s", *this, *current);
      this->_timer.async_wait(
        [delay, current, timeout_msg]
        (boost::system::error_code const& e)
        {
          if (e == boost::system::errc::operation_canceled)
            return;
          else if (e)
            ELLE_ABORT("unexpected timer error: %s", e);
          {
            ELLE_TRACE_SCOPE("%s", timeout_msg);
            current->raise<reactor::Timeout>(delay);
            if (current->state() == Thread::State::frozen)
              current->_wait_abort("guard timed out");
          }
        });
    }

    TimeoutGuard::~TimeoutGuard()
    {
      ELLE_TRACE_SCOPE("%s: cancel", *this);
      this->_timer.cancel();
    }

    void
    TimeoutGuard::print(std::ostream& output) const
    {
      elle::fprintf(output, "reactor::TimeoutGuard(%s)", this->_delay);
    }
  }
}
