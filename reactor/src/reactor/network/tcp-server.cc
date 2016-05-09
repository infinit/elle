#include <reactor/network/tcp-server.hh>
#include <reactor/network/tcp-socket.hh>
#include <reactor/scheduler.hh>

ELLE_LOG_COMPONENT("reactor.network.TCPServer");

namespace reactor
{
  namespace network
  {
    /*-------------.
    | Construction |
    `-------------*/
    TCPServer::TCPServer(bool no_delay)
      : Super()
      , _no_delay(no_delay)
    {}

    TCPServer::~TCPServer()
    {}

    /*----------.
    | Accepting |
    `----------*/

    void
    TCPServer::listen(int port)
    {
       listen(EndPoint(boost::asio::ip::tcp::v4(), port));
    }

    int
    TCPServer::port() const
    {
      return local_endpoint().port();
    }

    std::unique_ptr<TCPSocket>
    TCPServer::accept()
    {
      // Open a new raw socket.
      auto new_socket = elle::make_unique<AsioSocket>(
        reactor::Scheduler::scheduler()->io_service());
      EndPoint peer;
      this->_accept(*new_socket, peer);
      // Socket is now connected so make it into a TCPSocket.
      std::unique_ptr<TCPSocket> res(
        new TCPSocket(std::move(new_socket), peer));
      // TCP no delay disable Nagle's algorithm.
      if (this->_no_delay)
      {
        res->socket()->lowest_layer().set_option(
          boost::asio::ip::tcp::no_delay(true));
      }
      ELLE_TRACE("%s: got connection: %s", *this, *res);
      return res;
    }

    std::unique_ptr<Socket>
    TCPServer::_accept()
    {
      return this->accept();
    }
  }
}
