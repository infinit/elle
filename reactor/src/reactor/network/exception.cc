#include <reactor/network/exception.hh>

namespace reactor
{
  namespace network
  {
    Exception::Exception(Scheduler& scheduler, const std::string& message)
      : Super(scheduler, message)
    {}

    ConnectionClosed::ConnectionClosed(Scheduler& scheduler)
      : Super(scheduler, "connection closed")
    {}

    TimeOut::TimeOut(Scheduler& scheduler)
      : Super(scheduler, "network operation timed out")
    {}
  }
}
