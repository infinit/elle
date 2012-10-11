#include <elle/printf.hh>
#include <elle/idiom/Close.hh>

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
      , _acceptor(0)
    {}

    TCPServer::~TCPServer()
    {
      delete _acceptor;
    }

    /*----------.
    | Accepting |
    `----------*/

    class TCPAccept: public Operation
    {
      public:
        TCPAccept(Scheduler& scheduler, boost::asio::ip::tcp::acceptor& acceptor)
          : Operation(scheduler)
          , _acceptor(acceptor)
          , _socket(0)
          , _peer()
        {}

        virtual const char* type_name() const
        {
          static const char* name = "server accept";
          return name;
        }

        TCPSocket::AsioSocket* socket()
        {
          return _socket;
        }

      protected:
        virtual void _abort()
        {
          _acceptor.cancel();
          _signal();
        }

        virtual void _start()
        {
          _socket = new TCPSocket::AsioSocket(scheduler().io_service());
          _acceptor.async_accept(*_socket, _peer,
                                 boost::bind(&TCPAccept::_wakeup, this, _1));
        }

      private:

        void _wakeup(const boost::system::error_code& error)

        {
          if (error == boost::system::errc::operation_canceled)
            return;
          if (error)
            _raise(new Exception(scheduler(), error.message()));
          _signal();
        }

        boost::asio::ip::tcp::acceptor& _acceptor;
        TCPSocket::AsioSocket* _socket;
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
          _acceptor = new boost::asio::ip::tcp::acceptor(scheduler().io_service(),
                                                         end_point);
        }
      catch (boost::system::system_error& e)
        {
          throw Exception(scheduler(),
                          elle::sprintf("unable to listen on %s: %s",
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
        throw Exception(
            const_cast<TCPServer*>(this)->scheduler(), //XXX
            "The server is not listening.");
      return _acceptor->local_endpoint();
    }

  }
}
