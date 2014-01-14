#include <reactor/network/ssl-server.hh>

ELLE_LOG_COMPONENT("reactor.network.SSLServer");

namespace reactor
{
  namespace network
  {
    /*-------------.
    | Construction |
    `-------------*/
    SSLServer::SSLServer(std::unique_ptr<SSLCertificate> certificate):
      Super(*reactor::Scheduler::scheduler()),
      _certificate(std::move(certificate))
    {}

    SSLServer::~SSLServer()
    {}

    /*----------.
    | Accepting |
    `----------*/
    SSLSocket*
    SSLServer::accept()
    {
      // Open a new SSL Socket.
      auto ssl_stream = elle::make_unique<SSLStream>(
        reactor::Scheduler::scheduler()->io_service(),
        this->_certificate->context());
      ELLE_DUMP("%s: opened underlying TCP socket", *this);
      EndPoint peer;
      this->_accept(ssl_stream->next_layer(), peer);
      ELLE_DUMP("%s: got socket end point", *this);
      // SSL stream now has end point so we can make it an SSLSocket.
      SSLSocket* new_socket = new SSLSocket(std::move(ssl_stream),
                                            peer,
                                            this->_certificate);
      // Socket is now connected so we can do a handshake.
      new_socket->_server_handshake();
      ELLE_TRACE("%s: got connection: %s", *this, *new_socket);
      return new_socket;
    }
  }
}
