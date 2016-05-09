#ifndef INFINIT_REACTOR_NETWORK_TCP_SOCKET_HH
# define INFINIT_REACTOR_NETWORK_TCP_SOCKET_HH

# include <reactor/network/socket.hh>

namespace reactor
{
  namespace network
  {
    class TCPSocket
      : public StreamSocket<boost::asio::ip::tcp::socket>
    {
    /*---------.
    | Typedefs |
    `---------*/
    public:
      typedef StreamSocket<boost::asio::ip::tcp::socket> Super;
      typedef boost::asio::ip::tcp::resolver AsioResolver;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      TCPSocket(const std::string& hostname,
                const std::string& port,
                DurationOpt timeout = DurationOpt());
      TCPSocket(const std::string& hostname,
                int port,
                DurationOpt timeout = DurationOpt());
      TCPSocket(boost::asio::ip::tcp::endpoint const& endpoint,
                DurationOpt timeout = DurationOpt());
      virtual
      ~TCPSocket();
    private:
      friend class TCPServer;
      TCPSocket(std::unique_ptr<AsioSocket> socket,
                AsioSocket::endpoint_type const& endpoint);
    };
  }
}

#endif
