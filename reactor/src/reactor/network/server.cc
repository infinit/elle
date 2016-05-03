#include <reactor/network/exception.hh>
#include <reactor/network/server.hh>
#include <reactor/operation.hh>
#include <reactor/scheduler.hh>

ELLE_LOG_COMPONENT("reactor.network.Server");

namespace reactor
{
  namespace network
  {
    /*-------------.
    | Construction |
    `-------------*/
    Server::Server():
      Server(reactor::scheduler())
    {}

    Server::Server(Scheduler& scheduler):
      _scheduler(scheduler),
      _acceptor()
    {}

    Server::~Server()
    {}

    /*----------.
    | Listening |
    `----------*/
    void
    Server::listen(const EndPoint& end_point)
    {
      try
      {
        this->_acceptor.reset(new TCPAcceptor(
          this->_scheduler.io_service(), end_point));
      }
      catch (boost::system::system_error& e)
      {
        throw Exception(elle::sprintf("unable to listen on %s: %s",
                                      end_point, e.what()));
      }
    }

    class TCPAccept:
      public Operation
    {
    public:
      typedef boost::asio::ip::tcp::acceptor TCPAcceptor;
      typedef boost::asio::ip::tcp::endpoint EndPoint;

      TCPAccept(TCPSocket::AsioSocket& socket,
                EndPoint& peer,
                TCPAcceptor& acceptor):
        Operation(*reactor::Scheduler::scheduler()),
        _acceptor(acceptor),
        _peer(peer),
        _socket(socket)
      {}

      void
      print(std::ostream& stream) const
      {
        stream << "accept on " << this->_acceptor.local_endpoint();
      }

    private:
      void
      _abort()
      {
        this->_acceptor.cancel();
        this->_signal();
      }

      void
      _start()
      {
        this->_acceptor.async_accept(
          this->_socket,
          this->_peer,
          boost::bind(&TCPAccept::_wakeup, this, _1));
      }

      void
      _wakeup(const boost::system::error_code& error)
      {
        if (error == boost::system::errc::operation_canceled)
          return;
        if (error)
          _raise<Exception>(error.message());
        this->_signal();
      }

    private:
      ELLE_ATTRIBUTE_R(TCPAcceptor&, acceptor);
      ELLE_ATTRIBUTE(EndPoint&, peer);
      ELLE_ATTRIBUTE_X(TCPSocket::AsioSocket&, socket);
    };

    /*----------.
    | Accepting |
    `----------*/

    std::unique_ptr<Socket>
    Server::accept()
    {
      return this->_accept();
    }

    void
    Server::listen(int port, bool v6)
    {
      if (v6)
        listen(EndPoint(boost::asio::ip::address_v6::any(), port));
      else
        listen(EndPoint(boost::asio::ip::address_v4::any(), port));
    }

    void
    Server::_accept(TCPSocket::AsioSocket& socket, EndPoint& peer)
    {
      ELLE_TRACE_SCOPE("%s: wait for connection", *this);
      // FIXME: server should listen in ctor to avoid this crappy state ?
      ELLE_ASSERT_NEQ(this->acceptor(), nullptr);
      TCPAccept accept(socket, peer, *this->_acceptor);
      accept.run();
    }

    int
    Server::port() const
    {
      return local_endpoint().port();
    }

    Server::EndPoint
    Server::local_endpoint() const
    {
      if (this->_acceptor == nullptr)
        throw Exception("The server is not listening.");
      return this->_acceptor->local_endpoint();
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Server::print(std::ostream& stream) const
    {
      stream << "Server";
      if (this->_acceptor)
      {
        stream << "(" << this->_acceptor->local_endpoint() << ")";
      }
    }
  }
}
