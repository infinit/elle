#ifndef INFINIT_REACTOR_NETWORK_UDP_SERVER_SOCKET_HH
# define INFINIT_REACTOR_NETWORK_UDP_SERVER_SOCKET_HH

# include <reactor/network/fwd.hh>
# include <reactor/network/socket.hh>
# include <reactor/signal.hh>

namespace reactor
{
  namespace network
  {
    class UDPServerSocket: public Socket
    {
      /*---------.
      | Typedefs |
      `---------*/
      public:
        typedef Socket Super;
        typedef boost::asio::ip::udp::endpoint EndPoint;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        UDPServerSocket(Scheduler& sched,
                        UDPServer* server, const EndPoint& peer);
        virtual ~UDPServerSocket();

      /*-----.
      | Read |
      `-----*/
      public:
        virtual
        void
        read(Buffer buffer,
             DurationOpt timeout = DurationOpt(),
             int* bytes_read = nullptr) override;
        virtual
        Size
        read_some(Buffer buffer,
                  DurationOpt timeout = DurationOpt(),
                  int* bytes_read = nullptr) override;
      private:
        friend class UDPServer;
        UDPServer* _server;
        EndPoint _peer;
        Byte* _read_buffer;
        Size _read_buffer_capacity;
        Size _read_buffer_size;
        Signal _read_ready;

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
