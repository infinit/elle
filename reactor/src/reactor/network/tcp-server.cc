#include <elle/printf.hh>

#include <reactor/exception.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/operation.hh>
#include <reactor/scheduler.hh>

namespace reactor
{
  namespace network
  {

    /*-------------.
    | Construction |
    `-------------*/

    TCPServer::TCPServer(Scheduler& sched)
      : Super(sched)
      , _acceptor()
    {}

    TCPServer::~TCPServer()
    {}

    /*----------.
    | Accepting |
    `----------*/

    class TCPAccept: public Operation
    {
      public:
        TCPAccept(Scheduler& scheduler, boost::asio::ip::tcp::acceptor& acceptor)
          : Operation(scheduler)
          , _acceptor(acceptor)
          , _socket()
          , _peer()
        {}

        virtual const char* type_name() const
        {
          static const char* name = "server accept";
          return name;
        }

        TCPSocket::AsioSocket* socket()
        {
          return _socket.release();
        }

      protected:
        virtual void _abort()
        {
          _acceptor.cancel();
          _signal();
        }

        virtual void _start()
        {
          _socket.reset(new TCPSocket::AsioSocket(scheduler().io_service()));
          _acceptor.async_accept(*_socket, _peer,
                                 boost::bind(&TCPAccept::_wakeup, this, _1));
        }

      private:

        void _wakeup(const boost::system::error_code& error)

        {
          if (error == boost::system::errc::operation_canceled)
            return;
          if (error)
            _raise<Exception>(error.message());
          _signal();
        }

        boost::asio::ip::tcp::acceptor& _acceptor;
        std::unique_ptr<TCPSocket::AsioSocket> _socket;
        TCPSocket::EndPoint _peer;
    };

    TCPSocket*
    TCPServer::accept()
    {
      // FIXME: server should listen in ctor to avoid this crappy state ?
      assert(_acceptor);
      TCPAccept accept(scheduler(), *_acceptor);
      accept.run();
      TCPSocket* socket = new TCPSocket(scheduler(), accept.socket());
      return socket;
    }

    /*----------.
    | Listening |
    `----------*/

    void
    TCPServer::listen(const EndPoint& end_point)
    {
      try
        {
          _acceptor.reset
            (new boost::asio::ip::tcp::acceptor
             (scheduler().io_service(), end_point));
        }
      catch (boost::system::system_error& e)
        {
          throw Exception(elle::sprintf("unable to listen on %s: %s",
                                        end_point, e.what()));
        }
    }

    void
    TCPServer::listen(int port)
    {
      return listen(EndPoint(boost::asio::ip::tcp::v4(), port));
    }

    TCPServer::EndPoint
    TCPServer::local_endpoint() const
    {
      if (_acceptor == nullptr)
        throw Exception("The server is not listening.");
      return _acceptor->local_endpoint();
    }

    int
    TCPServer::port() const
    {
      return local_endpoint().port();
    }
  }
}
