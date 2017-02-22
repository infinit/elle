#include <elle/printf.hh>
#include <elle/reactor/network/exception.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      Exception::Exception(const std::string& message):
        Super(message)
      {}

      SocketClosed::SocketClosed()
        : Super("socket was closed")
      {}

      ConnectionRefused::ConnectionRefused()
        : Super("connection refused")
      {}

      PermissionDenied::PermissionDenied(std::string const& message)
        : Super(message)
      {}

      InvalidEndpoint::InvalidEndpoint(std::string const& message)
        : Super(message)
      {}

      ConnectionClosed::ConnectionClosed()
        : Super("connection closed")
      {}

      ConnectionClosed::ConnectionClosed(std::string const& message)
        : Super(elle::sprintf("connection closed: %s", message))
      {}

      SSLShortRead::SSLShortRead()
        : Super("SSL short read")
      {}

      ResolutionError::ResolutionError(std::string const& host,
                                       std::string const& message):
        Super(elle::sprintf("error resolving '%s': %s", host, message)),
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
}
