#ifndef INFINIT_REACTOR_NETWORK_UDT_SERVER_HH
# define INFINIT_REACTOR_NETWORK_UDT_SERVER_HH

# include <reactor/network/server.hh>
# include <reactor/network/udt-socket.hh>
# include <reactor/signal.hh>

# include <elle/Printable.hh>

namespace reactor
{
  namespace network
  {
    class UDTServer: public Server,
                     public ProtoServer<UDTSocket>,
                     public elle::Printable
    {
      public:
        typedef Server Super;
        UDTServer(Scheduler& sched);
        UDTServer(Scheduler& sched, std::unique_ptr<UDPSocket> sock);
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
        ELLE_ATTRIBUTE_X(std::unique_ptr<reactor::network::UDPSocket>, udp_socket);

      /*----------.
      | Printable |
      `----------*/
      public:
        virtual
        void
        print(std::ostream&) const;
    };
  }
}

#endif
