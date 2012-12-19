#ifndef INFINIT_REACTOR_NETWORK_UDT_SERVER_HH
# define INFINIT_REACTOR_NETWORK_UDT_SERVER_HH

# include <reactor/network/server.hh>
# include <reactor/network/udt-socket.hh>

# include <elle/nat/Nat.hh>

namespace reactor
{
  namespace network
  {
    class UDTServer: public Server, public ProtoServer<UDTSocket>
    {
      public:
        typedef Server Super;
        UDTServer(Scheduler& sched);
        virtual ~UDTServer();

      /*----------.
      | Listening |
      `----------*/
      public:
        void listen(int port = 0);
        void listen_fd(int port, int fd);

      public:
        /// The locally bound port.
        virtual int port() const;
        /// The locally bound endpoint.
        EndPoint local_endpoint() const;

      /*----------.
      | Accepting |
      `----------*/
      public:
        UDTSocket* accept();
      private:
        boost::asio::ip::udt::acceptor* _acceptor;

      /*----.
      | NAT |
      `----*/
      private:
        elle::nat::NAT _nat;
        std::shared_ptr<reactor::network::UDPSocket> _udp_socket;
    };
  }
}

#endif
