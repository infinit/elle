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
      Exception(Scheduler& scheduler, const std::string& message);
      INFINIT_REACTOR_EXCEPTION(Exception);
    };

    class ConnectionClosed: public Exception
    {
    public:
      typedef Exception Super;
      ConnectionClosed(Scheduler& scheduler);
      INFINIT_REACTOR_EXCEPTION(ConnectionClosed);
    };

    class TimeOut: public Exception
    {
    public:
      typedef Exception Super;
      TimeOut(Scheduler& scheduler);
      INFINIT_REACTOR_EXCEPTION(TimeOut);
    };
  }
}

#endif
