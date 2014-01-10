#ifndef INFINIT_REACTOR_NETWORK_SSL_SERVER_HH
# define INFINIT_REACTOR_NETWORK_SSL_SERVER_HH

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
      SSLServer(SSLCertificate const& certificate);
      SSLServer(Scheduler& scheduler,
                SSLCertificate const& certificate);

      virtual
      ~SSLServer();

    /*----------.
    | Accepting |
    `----------*/
    public:
      SSLSocket*
      accept();

    private:
      ELLE_ATTRIBUTE(SSLCertificate const&, certificate);
    };
  }
}

#endif // INFINIT_REACTOR_NETWORK_SSL_SERVER_HH
