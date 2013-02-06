#include <reactor/network/exception.hh>

namespace reactor
{
  namespace network
  {
    Exception::Exception(const std::string& message)
      : Super(message)
    {}

    ConnectionClosed::ConnectionClosed()
      : Super("connection closed")
    {}

    TimeOut::TimeOut()
      : Super("network operation timed out")
    {}
  }
}
