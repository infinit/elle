#ifndef INFINIT_REACTOR_NETWORK_EXCEPTION_HH
# define INFINIT_REACTOR_NETWORK_EXCEPTION_HH

# include <reactor/exception.hh>

namespace reactor
{
  namespace network
  {
    class Exception: public reactor::Exception
    {
      public:
        typedef reactor::Exception Super;
        Exception(const std::string& message);
        INFINIT_REACTOR_EXCEPTION(Exception);
    };

    class ConnectionClosed: public Exception
    {
      public:
        typedef Exception Super;
        ConnectionClosed();
        INFINIT_REACTOR_EXCEPTION(ConnectionClosed);
    };

    class TimeOut: public Exception
    {
      public:
        typedef Exception Super;
        TimeOut();
        INFINIT_REACTOR_EXCEPTION(TimeOut);
    };
  }
}

#endif
