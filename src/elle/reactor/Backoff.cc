#include <elle/reactor/Backoff.hh>
#include <elle/reactor/scheduler.hh>

namespace elle
{
  namespace reactor
  {
    Backoff::Backoff(Duration base, Duration  max)
      : _times_backed(0)
      , _base_backoff(base)
      , _max_backoff(max)
    {}

    void
    Backoff::backoff()
    {
      ++this->_times_backed;
      const auto delay =
        this->_base_backoff * int(std::pow(2, this->_times_backed));
      elle::reactor::sleep(std::min(delay, this->_max_backoff));
    }
  }
}
