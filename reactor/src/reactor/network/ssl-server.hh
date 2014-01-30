#ifndef INFINIT_REACTOR_NETWORK_SSL_SERVER_HH
# define INFINIT_REACTOR_NETWORK_SSL_SERVER_HH

# include <elle/utility/Duration.hh>

# include <reactor/Channel.hh>
# include <reactor/network/server.hh>
# include <reactor/network/ssl-socket.hh>

namespace reactor
{
  namespace network
  {
    class SSLServer:
      public Server,
      public ProtoServer<SSLSocket>
    {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      typedef Server Super;
      SSLServer(std::unique_ptr<SSLCertificate> certificate,
                reactor::Duration const& handshake_timeout = 30_sec);

      virtual
      ~SSLServer();

    /*----------.
    | Accepting |
    `----------*/
    public:
      std::unique_ptr<Socket>
      accept();

    private:
      void
      _handshake();
      ELLE_ATTRIBUTE(std::shared_ptr<SSLCertificate>, certificate);
      ELLE_ATTRIBUTE(reactor::Duration, handshake_timeout);
      ELLE_ATTRIBUTE(reactor::Channel<std::unique_ptr<SSLSocket>>, sockets);
      ELLE_ATTRIBUTE(reactor::Thread, handshake_thread);
    };
  }
}

#endif // INFINIT_REACTOR_NETWORK_SSL_SERVER_HH
