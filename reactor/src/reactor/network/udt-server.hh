#ifndef INFINIT_REACTOR_NETWORK_UDT_SERVER_HH
# define INFINIT_REACTOR_NETWORK_UDT_SERVER_HH

# include <reactor/network/server.hh>
# include <reactor/network/udt-socket.hh>
# include <reactor/signal.hh>

# include <elle/Printable.hh>

# include <asio-udt/acceptor.hh>

namespace reactor
{
  namespace network
  {
    class UDTServer:
      public Server,
      public ProtoServer<UDTSocket>,
      public elle::Printable
    {
    public:
      typedef Server Super;
      UDTServer(Scheduler& sched);
      virtual
      ~UDTServer();

      /*----------.
      | Listening |
      `----------*/
    public:
      void
      listen(int port = 0);

    public:
      /// The locally bound port.
      virtual
      int
      port() const;

      /// The locally bound endpoint.
      EndPoint
      local_endpoint() const;

    private:
      ELLE_ATTRIBUTE(std::unique_ptr<boost::asio::ip::udt::acceptor>, acceptor);

      /*----------.
      | Accepting |
      `----------*/
    public:
      virtual
      UDTSocket*
      accept();

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
