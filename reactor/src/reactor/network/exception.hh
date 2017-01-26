#ifndef INFINIT_REACTOR_NETWORK_EXCEPTION_HH
# define INFINIT_REACTOR_NETWORK_EXCEPTION_HH

# include <elle/attribute.hh>
# include <elle/Error.hh>

# include <reactor/exception.hh>

namespace reactor
{
  namespace network
  {
    class Exception
      : public elle::Error
    {
    public:
      typedef elle::Error Super;
      Exception(std::string const& message);
    };

    class SocketClosed
      : public Exception
    {
    public:
      typedef Exception Super;
      SocketClosed();
    };

    class ConnectionRefused
      : public Exception
    {
    public:
      typedef Exception Super;
      ConnectionRefused();
    };

    class PermissionDenied
      : public Exception
    {
    public:
      typedef Exception Super;
      PermissionDenied(std::string const& message);
    };

    class ConnectionClosed
      : public Exception
    {
    public:
      typedef Exception Super;
      ConnectionClosed();
      ConnectionClosed(std::string const& message);
    };

    class SSLShortRead
      : public ConnectionClosed
    {
    public:
      typedef ConnectionClosed Super;
      SSLShortRead();
    };

    class ResolutionError
      : public Exception
    {
    public:
      typedef Exception Super;
      ResolutionError(std::string const& host,
                      std::string const& message);

    private:
      ELLE_ATTRIBUTE_R(std::string, host);
    };

    class SSLCertificateError
      : public Exception
    {
    public:
      typedef Exception Super;
      SSLCertificateError(std::string const& message);
    };

    class SSLHandshakeError
      : public Exception
    {
    public:
      typedef Exception Super;
      SSLHandshakeError(std::string const& message);
    };

    class TimeOut
      : public Exception
    {
    public:
      typedef Exception Super;
      TimeOut();
    };
  }
}

#endif
