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

    SSLCertificateError::SSLCertificateError(std::string const& message):
      Super(elle::sprintf("SSL certificate error: %s", message))
    {}

    SSLHandshakeError::SSLHandshakeError(std::string const& message):
      Super(elle::sprintf("SSL handshake error: %s", message))
    {}

    TimeOut::TimeOut():
      Super("network operation timed out")
    {}
  }
}
