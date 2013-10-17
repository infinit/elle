#ifndef INFINIT_REACTOR_NETWORK_TCP_SOCKET_HH
# define INFINIT_REACTOR_NETWORK_TCP_SOCKET_HH

# include <reactor/asio.hh>
# include <reactor/mutex.hh>
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
      TCPSocket(const std::string& hostname,
                const std::string& port,
                DurationOpt timeout = DurationOpt());
      TCPSocket(Scheduler& sched,
                const std::string& hostname,
                const std::string& port,
                DurationOpt timeout = DurationOpt());
      TCPSocket(const std::string& hostname,
                int port,
                DurationOpt timeout = DurationOpt());
      TCPSocket(Scheduler& sched,
                const std::string& hostname,
                int port,
                DurationOpt timeout = DurationOpt());
      virtual ~TCPSocket();
      void
      close();
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
      elle::Buffer
      read_until(std::string const& delimiter,
                 DurationOpt opt = DurationOpt());
    private:
      virtual Size _read(Buffer buffer,
                         DurationOpt timeout,
                         bool some);

    /*------.
    | Write |
    `------*/
    public:
      virtual void write(elle::ConstWeakBuffer buffer);
    private:
      Mutex _write_mutex;

    /*----------------.
    | Pretty printing |
    `----------------*/
    public:
      void print(std::ostream& s) const;
    };
  }
}

#endif
