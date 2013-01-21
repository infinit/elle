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
        /// The public bound endpoint.
        ELLE_ATTRIBUTE_R(EndPoint, public_endpoint)

      /*----------.
      | Accepting |
      `----------*/
      public:
        virtual
        UDTSocket*
        accept();
        virtual
        void
        accept(std::string const& addr, int port);
      private:
        reactor::Signal _accepted;
        std::vector<std::unique_ptr<UDTSocket>> _sockets;
      /*----.
      | NAT |
      `----*/
      private:
        std::unique_ptr<reactor::network::UDPSocket> _udp_socket;
    };
  }
}

#endif
