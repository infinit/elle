#include <reactor/network/tcp-server.hh>
#include <reactor/scheduler.hh>

ELLE_LOG_COMPONENT("reactor.network.TCPServer");

namespace reactor
{
  namespace network
  {
    /*-------------.
    | Construction |
    `-------------*/
    TCPServer::TCPServer():
      Super()
    {}

    TCPServer::TCPServer(Scheduler& scheduler):
      Super(scheduler)
    {}

    TCPServer::~TCPServer()
    {}

    /*----------.
    | Accepting |
    `----------*/
    TCPSocket*
    TCPServer::accept()
    {
      // Open a new raw socket.
      auto new_socket = elle::make_unique<TCPSocket::AsioSocket>(
        this->_scheduler.io_service());
      EndPoint peer;
      this->_accept(*new_socket, peer);
      // Socket is now connected so make it into a TCPSocket.
      TCPSocket* res = new TCPSocket(this->_scheduler,
                                     std::move(new_socket),
                                     peer);
      ELLE_TRACE("%s: got connection: %s", *this, *res);
      return res;
    }
  }
}
