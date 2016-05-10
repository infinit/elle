#include <elle/utility/Move.hh>
#include <elle/With.hh>

#include <reactor/Scope.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/ssl-server.hh>

ELLE_LOG_COMPONENT("reactor.network.SSLServer");

namespace reactor
{
  namespace network
  {
    /*-------------.
    | Construction |
    `-------------*/

    SSLServer::SSLServer(std::unique_ptr<SSLCertificate> certificate,
                         reactor::Duration const& handshake_timeout)
      : Super()
      , _certificate(std::move(certificate))
      , _handshake_timeout(handshake_timeout)
      , _sockets()
      , _handshake_thread(elle::sprintf("%s handshake", *this),
                          std::bind(&SSLServer::_handshake,
                                    std::ref(*this)))
      , _shutdown_asynchronous(false)
    {}

    SSLServer::~SSLServer()
    {
      this->_handshake_thread.terminate_now();
    }

    /*----------.
    | Accepting |
    `----------*/

    void
    SSLServer::listen(int port)
    {
      this->listen(EndPoint(boost::asio::ip::tcp::v4(), port));
    }

    SSLServer::EndPoint
    SSLServer::_default_endpoint() const
    {
      return EndPoint(boost::asio::ip::tcp::v4(), 0);
    }

    int
    SSLServer::port() const
    {
      return local_endpoint().port();
    }

    void
    SSLServer::_handshake()
    {
      elle::With<reactor::Scope>() << [this] (reactor::Scope& scope)
      {
        while (true)
        {
          auto& io_service = reactor::Scheduler::scheduler()->io_service();
          auto ssl =
            elle::make_unique<SSLStream>(io_service,
                                         this->_certificate->context());
          EndPoint peer;
          this->_accept(ssl->next_layer(), peer);
          ELLE_TRACE("%s: got TCP connection", *this);
          auto naked = new SSLSocket(std::move(ssl),
                                     peer,
                                     this->_certificate,
                                     this->_handshake_timeout);
          naked->shutdown_asynchronous(this->_shutdown_asynchronous);
          auto socket =
            elle::utility::move_on_copy(std::unique_ptr<SSLSocket>(naked));
          scope.run_background(
            elle::sprintf("%s: handshake %s", *this, *naked),
            [this, socket]
            {
              ELLE_TRACE_SCOPE("%s: handshake %s", *this, *socket.value);
              try
              {
                socket->_server_handshake(this->_handshake_timeout);
                this->_sockets.put(socket);
              }
              catch (reactor::network::TimeOut const&)
              {
                ELLE_TRACE("%s: SSL handshake timed out for %s",
                           *this, *socket.value);
              }
              catch (reactor::network::SSLHandshakeError const& e)
              {
                ELLE_TRACE("%s: SSL handshake failed for %s: %s",
                           *this, *socket.value, e);
              }
            });
        }
      };
    }

    std::unique_ptr<SSLSocket>
    SSLServer::accept()
    {
      return this->_sockets.get();
    }

    std::unique_ptr<Socket>
    SSLServer::_accept()
    {
      return this->accept();
    }
  }
}
