#include <elle/printf.hh>
#include <elle/idiom/Close.hh>

#include <asio-udt/acceptor.hh>
#include <reactor/exception.hh>
#include <reactor/network/udt-server.hh>
#include <reactor/operation.hh>
#include <reactor/scheduler.hh>

namespace reactor
{
  namespace network
  {

    /*-------------.
    | Construction |
    `-------------*/

    UDTServer::UDTServer(Scheduler& sched)
      : Super(sched)
      , _acceptor(0)
    {}

    UDTServer::~UDTServer()
    {
      delete _acceptor;
    }

    /*----------.
    | Accepting |
    `----------*/

    class UDTAccept: public Operation
    {
      public:
        UDTAccept(Scheduler& scheduler, boost::asio::ip::udt::acceptor& acceptor)
          : Operation(scheduler)
          , _acceptor(acceptor)
          , _socket(0)
        {}

        virtual const char* type_name() const
        {
          static const char* name = "server accept";
          return name;
        }

        UDTSocket::AsioSocket* socket()
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
          _socket = new UDTSocket::AsioSocket(scheduler().io_service());
          _acceptor.async_accept(boost::bind(&UDTAccept::_wakeup,
                                             this, _1, _2));
        }

      private:

      void _wakeup(boost::system::error_code const& error,
                   boost::asio::ip::udt::socket* socket)

        {
          if (error == boost::system::errc::operation_canceled)
            return;
          if (error)
            _raise(new Exception(scheduler(), error.message()));
          _socket = socket;
          _signal();
        }

        boost::asio::ip::udt::acceptor& _acceptor;
        UDTSocket::AsioSocket* _socket;
    };

    UDTSocket*
    UDTServer::accept()
    {
      // FIXME: server should listen in ctor to avoid this crappy state ?
      assert(_acceptor);
      UDTAccept accept(scheduler(), *_acceptor);
      accept.run();
      UDTSocket* socket = new UDTSocket(scheduler(), accept.socket());
      return socket;
    }

    /*----------.
    | Listening |
    `----------*/

    void
    UDTServer::listen(int port)
    {
      try
        {
          _acceptor = new boost::asio::ip::udt::acceptor
            (scheduler().io_service(), port);
        }
      catch (boost::system::system_error& e)
        {
          throw Exception(scheduler(),
                          elle::sprintf("unable to listen on %s: %s",
                                        port, e.what()));
        }
    }

    UDTServer::EndPoint
    UDTServer::local_endpoint() const
    {
      if (_acceptor == nullptr)
        throw Exception(
            const_cast<UDTServer*>(this)->scheduler(), //XXX
            "The server is not listening.");
      return EndPoint(boost::asio::ip::address_v4(), _acceptor->port());
    }

    int
    UDTServer::port() const
    {
      return local_endpoint().port();
    }

  }
}
