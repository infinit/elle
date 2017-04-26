#include <elle/reactor/network/TCPServer.hh>
#include <elle/reactor/network/TCPSocket.hh>
#include <elle/reactor/scheduler.hh>

ELLE_LOG_COMPONENT("elle.reactor.network.TCPServer");

namespace elle
{
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

      /*----------.
      | Accepting |
      `----------*/

      void
      TCPServer::listen(int port, bool enable_ipv6)
      {
        if (enable_ipv6)
          this->listen(EndPoint(boost::asio::ip::tcp::v6(), port));
        else
          this->listen(EndPoint(boost::asio::ip::tcp::v4(), port));
      }

      void
      TCPServer::listen(boost::asio::ip::address host,
                        int port, bool enable_ipv6)
      {
        if (enable_ipv6 && host.is_v4())
          host = boost::asio::ip::address_v6::v4_mapped(host.to_v4());
        this->listen(EndPoint(host, port));
      }

      auto
      TCPServer::_default_endpoint() const
        -> EndPoint
      {
        return {boost::asio::ip::tcp::v4(), 0};
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
        //
        // We can neither directly build the std::unique_ptr here, nor
        // use std::make_unique (which makes sense, as std::make_unique
        // being inline, it should produce the same code), because it
        // crashes on Windows builds for (very) obscure reasons.
        auto new_socket = elle::make_unique<AsioSocket>
          (reactor::Scheduler::scheduler()->io_service());
        EndPoint peer;
        this->_accept(*new_socket, peer);
        // Socket is now connected so make it into a TCPSocket.
        //
        // Cannot use make_unique: private ctor.
        auto res = std::unique_ptr<TCPSocket>
          (new TCPSocket(std::move(new_socket), peer));
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
}
