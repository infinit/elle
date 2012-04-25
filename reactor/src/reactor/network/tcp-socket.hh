#ifndef INFINIT_REACTOR_NETWORK_TCP_SOCKET_HH
# define INFINIT_REACTOR_NETWORK_TCP_SOCKET_HH

# include <reactor/network/socket.hh>

namespace reactor
{
  namespace network
  {
    class TCPSocket: public PlainSocket<boost::asio::ip::tcp::socket>
    {
    /*---------.
    | Typedefs |
    `---------*/
    public:
      typedef PlainSocket<boost::asio::ip::tcp::socket> Super;
      typedef boost::asio::ip::tcp::resolver AsioResolver;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      TCPSocket(Scheduler& sched,
                const std::string& hostname, const std::string& port);
      TCPSocket(Scheduler& sched,
                const std::string& hostname, int port);
      virtual ~TCPSocket();
    private:
      friend class TCPServer;
      TCPSocket(Scheduler& sched, AsioSocket* socket);

    /*-----.
    | Read |
    `-----*/
    public:
      virtual void read(Buffer buffer,
                        DurationOpt timeout = DurationOpt());
      virtual Size read_some(Buffer buffer,
                             DurationOpt timeout = DurationOpt());

    /*------.
    | Write |
    `------*/
    public:
      virtual void write(Buffer buffer);
      using Socket::write;

    /*-----------.
    | Properties |
    `-----------*/
    public:
      EndPoint Peer() const;

    /*----------------.
    | Pretty printing |
    `----------------*/
    public:
      void print(std::ostream& s) const;
    };
  }
}

#endif
