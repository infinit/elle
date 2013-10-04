#include <elle/printf.hh>

#include <reactor/network/exception.hh>

namespace reactor
{
  namespace network
  {
    Exception::Exception(const std::string& message):
      Super(message)
    {}

    ConnectionClosed::ConnectionClosed()
      : Super("connection closed")
    {}

    ResolutionError::ResolutionError(std::string const& host,
                                     std::string const& message):
      Super(elle::sprintf("error resolving %s: %s", host, message)),
      _host(host)
    {}


    TimeOut::TimeOut():
      Super("network operation timed out")
    {}
  }
}
