#include <elle/reactor/network/exception.hh>
#include <elle/reactor/network/server.hh>
#include <elle/reactor/network/ssl-socket.hh>
#include <elle/reactor/network/tcp-socket.hh>
#include <elle/reactor/operation.hh>
#include <elle/reactor/scheduler.hh>

ELLE_LOG_COMPONENT("elle.reactor.network.Server");

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /*-------------.
      | Construction |
      `-------------*/

      Server::Server()
        : Server(reactor::scheduler())
      {}

      Server::Server(Scheduler& scheduler)
        : _scheduler(scheduler)
      {}

      Server::~Server()
      = default;

      /*----------.
      | Listening |
      `----------*/

      template <typename Socket, typename EndPoint, typename Acceptor>
      void
      ProtoServer<Socket, EndPoint, Acceptor>::listen(EndPoint const& end_point)
      {
        try
        {
          this->_acceptor.reset(new Acceptor(
            this->_scheduler.io_service(), end_point));
        }
        catch (boost::system::system_error& e)
        {
          auto message =
            elle::sprintf("unable to listen on %s: %s", end_point, e.what());
          if (e.code() == boost::system::errc::permission_denied)
            throw PermissionDenied(message);
          throw Exception(message);
        }
      }

      template <typename Socket, typename EndPoint, typename Acceptor>
      void
      ProtoServer<Socket, EndPoint, Acceptor>::listen()
      {
        this->listen(this->_default_endpoint());
      }

      template <typename Socket, typename EndPoint, typename Acceptor>
      class Accept:
        public Operation
      {
      public:
        Accept(Socket& socket,
               EndPoint& peer,
               Acceptor& acceptor)
          : Operation(*reactor::Scheduler::scheduler())
          , _acceptor(acceptor)
          , _peer(peer)
          , _socket(socket)
        {}

        void
        print(std::ostream& stream) const override
        {
          elle::fprintf(stream, "accept on %s", this->_acceptor.local_endpoint());
        }

      private:
        void
        _abort() override
        {
          this->_acceptor.cancel();
          this->_signal();
        }

        void
        _start() override
        {
          this->_acceptor.async_accept(
            this->_socket,
            this->_peer,
            boost::bind(&Accept::_wakeup, this, _1));
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
        ELLE_ATTRIBUTE_R(Acceptor&, acceptor);
        ELLE_ATTRIBUTE(EndPoint&, peer);
        ELLE_ATTRIBUTE_X(Socket&, socket);
      };

      /*----------.
      | Accepting |
      `----------*/

      std::unique_ptr<Socket>
      Server::accept()
      {
        return this->_accept();
      }

      template <typename Socket, typename EndPoint, typename Acceptor>
      void
      ProtoServer<Socket, EndPoint, Acceptor>::_accept(
        AsioSocket& socket, EndPoint& peer)
      {
        ELLE_TRACE_SCOPE("%s: wait for connection", *this);
        // FIXME: server should listen in ctor to avoid this crappy state ?
        ELLE_ASSERT_NEQ(this->acceptor(), nullptr);
        Accept<Socket, EndPoint, Acceptor> accept(socket, peer, *this->_acceptor);
        accept.run();
      }

      template <typename Socket, typename EndPoint, typename Acceptor>
      EndPoint
      ProtoServer<Socket, EndPoint, Acceptor>::local_endpoint() const
      {
        if (this->_acceptor == nullptr)
          throw Exception("server is not listening");
        return this->_acceptor->local_endpoint();
      }

      /*----------.
      | Printable |
      `----------*/

      template <typename Socket, typename EndPoint, typename Acceptor>
      void
      ProtoServer<Socket, EndPoint, Acceptor>::print(std::ostream& stream) const
      {
        stream << "Server";
        if (this->_acceptor)
        {
          stream << "(" << this->_acceptor->local_endpoint() << ")";
        }
      }

      /*------------------------.
      | Explicit instantiations |
      `------------------------*/

      template
      class ProtoServer<boost::asio::ip::tcp::socket,
                        boost::asio::ip::tcp::endpoint,
                        boost::asio::ip::tcp::acceptor>;
#ifdef REACTOR_NETWORK_UNIX_DOMAIN_SOCKET
      template
      class ProtoServer<boost::asio::local::stream_protocol::socket,
                        boost::asio::local::stream_protocol::endpoint,
                        boost::asio::local::stream_protocol::acceptor>;
#endif
    }
  }
}
