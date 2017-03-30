#pragma once

#include <elle/Error.hh>
#include <elle/attribute.hh>
#include <elle/reactor/exception.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      class Error
        : public elle::Error
      {
      public:
        using Super = elle::Error;
        Error(std::string const& message);
      };

      using Exception [[deprecated("use elle::reactor::Error instead")]]
      = Error;

      class SocketClosed
        : public Error
      {
      public:
        using Super = Error;
        SocketClosed();
      };

      class ConnectionRefused
        : public Error
      {
      public:
        using Super = Error;
        ConnectionRefused();
      };

      class PermissionDenied
        : public Error
      {
      public:
        using Super = Error;
        PermissionDenied(std::string const& message);
      };

      class InvalidEndpoint
        : public Error
      {
      public:
        using Super = Error;
        InvalidEndpoint(std::string const& message);
      };

      class ConnectionClosed
        : public Error
      {
      public:
        using Super = Error;
        ConnectionClosed();
        ConnectionClosed(std::string const& message);
      };

      class SSLShortRead
        : public ConnectionClosed
      {
      public:
        using Super = ConnectionClosed;
        SSLShortRead();
      };

      class ResolutionError
        : public Error
      {
      public:
        using Super = Error;
        ResolutionError(std::string const& host,
                        std::string const& message);

      private:
        ELLE_ATTRIBUTE_R(std::string, host);
      };

      class SSLCertificateError
        : public Error
      {
      public:
        using Super = Error;
        SSLCertificateError(std::string const& message);
      };

      class SSLHandshakeError
        : public Error
      {
      public:
        using Super = Error;
        SSLHandshakeError(std::string const& message);
      };

      class TimeOut
        : public Error
      {
      public:
        using Super = Error;
        TimeOut();
      };
    }
  }
}
