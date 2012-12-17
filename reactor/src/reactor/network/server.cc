#include <reactor/network/server.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/network/udt-server.hh>

namespace reactor
{
  namespace network
  {
    Server::Server(Scheduler& sched)
      : _sched(sched)
    {}

    Server::~Server()
    {}

    Scheduler&
    Server::scheduler()
    {
      return _sched;
    }

    std::unique_ptr<Server>
    Server::create(Protocol protocol, Scheduler& sched)
    {
      switch (protocol)
        {
          case Protocol::tcp:
            return std::unique_ptr<Server>(new TCPServer(sched));
          case Protocol::udt:
            return std::unique_ptr<Server>(new UDTServer(sched));
          default:
            elle::unreachable();
        }
    }
  }
}
