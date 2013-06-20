#ifndef INFINIT_REACTOR_NETWORK_UDT_SOCKET_HH
# define INFINIT_REACTOR_NETWORK_UDT_SOCKET_HH

# include <asio-udt/socket.hh>
# include <reactor/mutex.hh>
# include <reactor/network/socket.hh>

namespace reactor
{
  namespace network
  {
    class UDTSocket: public PlainSocket<boost::asio::ip::udt::socket>
    {
    /*---------.
    | Typedefs |
    `---------*/
    public:
      typedef PlainSocket<boost::asio::ip::udt::socket> Super;
      // typedef boost::asio::ip::udt::resolver AsioResolver;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      UDTSocket(Scheduler& sched,
                const std::string& hostname,
                const std::string& port,
                DurationOpt timeout = DurationOpt());
      UDTSocket(Scheduler& sched,
                UDPSocket& socket,
                std::string const& hostname,
                std::string const& port,
                DurationOpt timeout = DurationOpt());
      UDTSocket(Scheduler& sched,
                int fd,
                const std::string& hostname,
                const std::string& port,
                DurationOpt timeout = DurationOpt());
      UDTSocket(Scheduler& sched,
                const std::string& hostname,
                int port,
                DurationOpt timeout = DurationOpt());
      UDTSocket(Scheduler& sched,
                const std::string& hostname,
                int port,
                int local_port,
                DurationOpt timeout = DurationOpt());
      UDTSocket(Scheduler& sched,
                UDPSocket& socket);
      virtual ~UDTSocket();
    private:
      UDTSocket(Scheduler& sched, AsioSocket* socket);
      friend class UDTServer;

    /*--------.
    | Connect |
    `--------*/
    public:
      void
      connect(std::string const& host,
              int port);

    /*-----.
    | Read |
    `-----*/
    public:
      virtual void read(Buffer buffer,
                        DurationOpt timeout = DurationOpt());
      virtual Size read_some(Buffer buffer,
                             DurationOpt timeout = DurationOpt());
    private:
      virtual Size _read(Buffer buffer,
                         DurationOpt timeout,
                         bool some);

    /*------.
    | Write |
    `------*/
    public:
      virtual void write(Buffer buffer);
      using Socket::write;
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
