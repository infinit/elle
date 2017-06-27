#include <elle/reactor/exception.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/reactor/sleep.hh>

namespace elle
{
  namespace reactor
  {
    Sleep::Sleep(Scheduler& scheduler, Duration d)
      : Operation(scheduler)
      , _duration(d)
      , _timer(scheduler.io_service(), d)
    {}

    /*----------.
    | Printable |
    `----------*/

    void
    Sleep::print(std::ostream& stream) const
    {
      stream << "sleep for " << this->_duration;
    }

    void
    Sleep::_abort()
    {
      _timer.cancel();
      _signal();
    }

    void
    Sleep::_start()
    {
      _timer.async_wait([this](const boost::system::error_code& error)
                        {
                          if (error == boost::asio::error::operation_aborted)
                            return;
                          if (error)
                            _raise<Exception>(error.message());
                          this->_signal();
                        });
    }
  }
}
