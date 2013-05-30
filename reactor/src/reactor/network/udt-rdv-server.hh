#ifndef INFINIT_REACTOR_NETWORK_UDT_RDV_SERVER_HH
# define INFINIT_REACTOR_NETWORK_UDT_RDV_SERVER_HH

# include <reactor/network/server.hh>
# include <reactor/network/udt-socket.hh>
# include <reactor/signal.hh>

# include <elle/Printable.hh>

# include <asio-udt/acceptor.hh>

namespace reactor
{
  namespace network
  {
    class UDTRendezVousServer:
      public Server,
      public ProtoServer<UDTSocket>,
      public elle::Printable
    {
    public:
      typedef Server Super;
      UDTRendezVousServer(Scheduler& sched);
      UDTRendezVousServer(Scheduler& sched,
                          std::unique_ptr<UDPSocket> sock);
      virtual ~UDTRendezVousServer();

      /*----------.
      | Listening |
      `----------*/
    public:
      void
      listen(int port = 0);
      void
      listen_fd(int port,
                int fd);

    public:
      /// The locally bound port.
      virtual
      int
      port() const;
      /// The locally bound endpoint.
      EndPoint
      local_endpoint() const;

      /*----------.
      | Accepting |
      `----------*/
    public:
      virtual
      UDTSocket*
      accept();

      virtual
      void
      accept(std::string const& addr,
             int port);

    private:
      ELLE_ATTRIBUTE(reactor::Signal, accepted);
      ELLE_ATTRIBUTE(std::vector<std::unique_ptr<UDTSocket>>, sockets);

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
