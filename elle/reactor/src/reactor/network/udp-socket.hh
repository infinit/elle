#ifndef INFINIT_REACTOR_NETWORK_UDP_SOCKET_HH
# define INFINIT_REACTOR_NETWORK_UDP_SOCKET_HH

# include <reactor/asio.hh>
# include <reactor/network/socket.hh>
# include <reactor/signal.hh>

namespace reactor
{
  namespace network
  {
    class UDPSocket: public PlainSocket<boost::asio::ip::udp::socket>
    {
      /*---------.
      | Typedefs |
      `---------*/
      public:
        typedef PlainSocket<boost::asio::ip::udp::socket> Super;
        typedef boost::asio::ip::udp::resolver AsioResolver;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        UDPSocket(Scheduler& sched,
                  const std::string& hostname, const std::string& port);
        UDPSocket(Scheduler& sched,
                  const std::string& hostname, int port);
        virtual ~UDPSocket();

      /*-----------.
      | Connection |
      `-----------*/
      public:
        void connect(const std::string& hostname, int port);
        void connect(const std::string& hostname,
                     const std::string& service);

      /*-----.
      | Read |
      `-----*/
      public:
        virtual Size read_some(Buffer buffer,
                               DurationOpt timeout = DurationOpt());

      /*------.
      | Write |
      `------*/
      public:
        virtual void write(Buffer buffer);
        using Super::write;

      /*----------------.
      | Pretty printing |
      `----------------*/
      public:
        void print(std::ostream& s) const;
    };
  }
}

#endif
