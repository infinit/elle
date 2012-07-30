#include <reactor/network/server.hh>

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
  }
}
