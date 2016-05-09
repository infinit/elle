#ifndef INFINIT_REACTOR_NETWORK_TCP_SERVER_HH
# define INFINIT_REACTOR_NETWORK_TCP_SERVER_HH

# include <reactor/network/server.hh>

namespace reactor
{
  namespace network
  {
    class TCPServer
      : public Server
      , public ProtoServer<TCPSocket>
    {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      typedef Server Super;
      TCPServer(bool no_delay = false);
      virtual
      ~TCPServer();
      std::unique_ptr<TCPSocket>
      accept();

    /*----------.
    | Accepting |
    `----------*/
    protected:
      using Super::_accept;
      virtual
      std::unique_ptr<Socket>
      _accept() override;

      ELLE_ATTRIBUTE_RX(bool, no_delay);
    };
  }
}

#endif // INFINIT_REACTOR_NETWORK_TCP_SERVER_HH
