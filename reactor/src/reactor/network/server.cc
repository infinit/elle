#include <reactor/network/server.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/network/udt-server.hh>
#include <reactor/network/udt-rdv-server.hh>

namespace reactor
{
  namespace network
  {
    Server::Server(Scheduler& sched)
      : _sched(sched)
    {}

    Server::~Server()
    {}

    void
    Server::accept(std::string const&, int)
    {
      // Nothing.
    }

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
            return std::unique_ptr<Server>(new UDTRendezVousServer(sched));
          default:
            elle::unreachable();
        }
    }
  }
}
