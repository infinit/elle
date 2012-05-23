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
            _raise(new Exception(error.message()));
          _signal();
        }

        boost::asio::ip::tcp::acceptor& _acceptor;
        TCPSocket::AsioSocket* _socket;
        TCPSocket::EndPoint _peer;
    };

    TCPSocket*
    TCPServer::accept()
    {
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
      _acceptor = new boost::asio::ip::tcp::acceptor(scheduler().io_service(),
                                                     end_point);
    }

    void
    TCPServer::listen(int port)
    {
      return listen(EndPoint(boost::asio::ip::tcp::v4(), port));
    }
  }
}
