#pragma once

# include <elle/Error.hh>
# include <elle/attribute.hh>
# include <elle/reactor/exception.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      class Exception
        : public elle::Error
      {
      public:
        using Super = elle::Error;
        Exception(std::string const& message);
      };

      class SocketClosed
        : public Exception
      {
      public:
        using Super = Exception;
        SocketClosed();
      };

      class ConnectionRefused
        : public Exception
      {
      public:
        using Super = Exception;
        ConnectionRefused();
      };

      class PermissionDenied
        : public Exception
      {
      public:
        using Super = Exception;
        PermissionDenied(std::string const& message);
      };

      class InvalidEndpoint
        : public Exception
      {
      public:
        using Super = Exception;
        InvalidEndpoint(std::string const& message);
      };

      class ConnectionClosed
        : public Exception
      {
      public:
        using Super = Exception;
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
        : public Exception
      {
      public:
        using Super = Exception;
        ResolutionError(std::string const& host,
                        std::string const& message);

      private:
        ELLE_ATTRIBUTE_R(std::string, host);
      };

      class SSLCertificateError
        : public Exception
      {
      public:
        using Super = Exception;
        SSLCertificateError(std::string const& message);
      };

      class SSLHandshakeError
        : public Exception
      {
      public:
        using Super = Exception;
        SSLHandshakeError(std::string const& message);
      };

      class TimeOut
        : public Exception
      {
      public:
        using Super = Exception;
        TimeOut();
      };
    }
  }
}
