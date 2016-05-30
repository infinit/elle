#ifndef INFINIT_REACTOR_NETWORK_UDP_SERVER_HH
# define INFINIT_REACTOR_NETWORK_UDP_SERVER_HH

# include <unordered_map>

# include <reactor/network/server.hh>
# include <reactor/network/udp-socket.hh>
# include <reactor/network/udp-server-socket.hh>
# include <reactor/signal.hh>

namespace reactor
{
  namespace network
  {
    struct HashEndpoint
    {
        std::size_t operator()(const boost::asio::ip::udp::endpoint&) const;
        std::size_t operator()(const boost::asio::ip::tcp::endpoint&) const;
    };

    class UDPServer: public Server, public ProtoServer<UDPSocket>
    {
      public:
        typedef Server Super;
        UDPServer(Scheduler& sched);
        ~UDPServer();

      /*----------.
      | Listening |
      `----------*/
      public:
        void listen(int port, bool enable_ipv6=false);
        void listen(const EndPoint& end_point);

      /*----------.
      | Accepting |
      `----------*/
      public:
        UDPServerSocket* accept();
      private:
        friend class UDPServerSocket;
        boost::asio::ip::udp::socket* _acceptor;

      /*---------------.
      | Demultiplexing |
      `---------------*/
      private:
        void _receive();
        void _receive_handle(const boost::system::error_code& error,
                             std::size_t bytes_transferred);
        char _buffer[512];
        EndPoint _peer;
        typedef std::unordered_map<EndPoint, UDPServerSocket*,
                                     HashEndpoint> Clients;
        Clients _clients;
        Signal _accept;
        typedef std::vector<UDPServerSocket*> Sockets;
        Sockets _accepted;

      /*---------.
      | Printing |
      `---------*/
      public:
        void print(std::ostream& s) const;
    };

    std::ostream& operator << (std::ostream& s, const UDPServer& server);
  }
}

#endif
