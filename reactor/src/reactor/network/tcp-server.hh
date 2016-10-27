#ifndef INFINIT_REACTOR_NETWORK_TCP_SERVER_HH
# define INFINIT_REACTOR_NETWORK_TCP_SERVER_HH

# include <reactor/network/server.hh>

namespace reactor
{
  namespace network
  {
    class TCPServer
      : public ProtoServer<boost::asio::ip::tcp::socket,
                           boost::asio::ip::tcp::endpoint,
                           boost::asio::ip::tcp::acceptor>
    {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      typedef ProtoServer<boost::asio::ip::tcp::socket,
                          boost::asio::ip::tcp::endpoint,
                          boost::asio::ip::tcp::acceptor> Super;
      TCPServer(bool no_delay = false);
      virtual
      ~TCPServer();
      std::unique_ptr<TCPSocket>
      accept();

    /*----------.
    | Accepting |
    `----------*/
    public:
      using Super::listen;
      void
      listen(int port, bool enable_ipv6=false);
      void
      listen(boost::asio::ip::address host, int port = 0, bool enable_ipv6 = false);
      int
      port() const;
    protected:
      virtual
      EndPoint
      _default_endpoint() const override;
      using Super::_accept;
      virtual
      std::unique_ptr<Socket>
      _accept() override;
      ELLE_ATTRIBUTE_RX(bool, no_delay);
    };
  }
}

#endif // INFINIT_REACTOR_NETWORK_TCP_SERVER_HH
