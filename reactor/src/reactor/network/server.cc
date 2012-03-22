#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <reactor/network/server.hh>
#include <reactor/network/socket.hh>
#include <reactor/operation.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

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
