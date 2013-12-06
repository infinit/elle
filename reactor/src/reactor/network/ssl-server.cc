#include <reactor/network/ssl-server.hh>
#include <reactor/operation.hh>
#include <reactor/network/socket.hh>

namespace reactor
{
  namespace network
  {
    SSLServer::SSLServer(SSLCertif& cert):
      SSLServer(*reactor::Scheduler::scheduler(), cert)
    {}

    SSLServer::SSLServer(reactor::Scheduler& sched,
                         SSLCertif& cert):
      Super(sched),
      _cert(cert)
    {
      ELLE_ASSERT(&sched);
    }

    SSLServer::~SSLServer()
    {}

    /*----------.
    | Accepting |
    `----------*/

    class SSLAccept: public Operation
    {
    public:
      SSLAccept(Scheduler& scheduler, boost::asio::ip::tcp::acceptor& acceptor,
                SSLCertif& cert):
        Operation(scheduler),
        _acceptor(acceptor),
        _peer(),
        _socket(nullptr),
        _cert(cert)
      {}

      virtual
      void
      print(std::ostream& stream) const override
      {
        stream << "accept on " << this->_acceptor.local_endpoint();
      }

    protected:
      virtual void _abort()
      {
        _acceptor.cancel();
        _signal();
      }

      virtual void _start()
      {
        typedef SocketSpecialization<
          boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> Spe;

        if (_socket != nullptr)
        {
          delete _socket;
          _socket = nullptr;
        }

        _socket = new SSLSocket(scheduler(), _peer, _cert);
        _acceptor.async_accept(Spe::socket(*_socket->socket()), _peer,
                               boost::bind(&SSLAccept::_wakeup, this, _1));
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

      ELLE_ATTRIBUTE(boost::asio::ip::tcp::acceptor&, acceptor);
      ELLE_ATTRIBUTE_R(SSLSocket::EndPoint, peer);
      ELLE_ATTRIBUTE_X(SSLSocket*, socket);
      ELLE_ATTRIBUTE(SSLCertif, cert);
    };

    /*----------.
    | Listening |
    `----------*/

    void
    SSLServer::listen(int port)
    {
      return listen(EndPoint(boost::asio::ip::tcp::v4(), port));
    }

    void
    SSLServer::listen(const EndPoint& end_point)
    {
      try
        {
          _acceptor.reset(new boost::asio::ip::tcp::acceptor(
            scheduler().io_service(), end_point));
        }
      catch (boost::system::system_error& e)
        {
          throw Exception(elle::sprintf("unable to listen on %s: %s",
                                        end_point, e.what()));
        }
    }

    SSLSocket*
    SSLServer::accept()
    {
      // FIXME: server should listen in ctor to avoid this crappy state ?
      ELLE_ASSERT_NEQ(_acceptor, nullptr);
      SSLAccept accept(scheduler(), *_acceptor, _cert);
      accept.run();
      accept.socket()->server_handshake();
      return accept.socket();
    }

    SSLServer::EndPoint
    SSLServer::local_endpoint() const
    {
      if (_acceptor == nullptr)
        throw Exception("The server is not listening.");
      return _acceptor->local_endpoint();
    }

    int
    SSLServer::port() const
    {
      return local_endpoint().port();
    }
  }
}
