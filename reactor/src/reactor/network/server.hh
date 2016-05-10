#ifndef INFINIT_REACTOR_NETWORK_SERVER_HH
# define INFINIT_REACTOR_NETWORK_SERVER_HH

# include <memory>

# include <reactor/asio.hh>
# include <reactor/network/fwd.hh>
# include <reactor/network/Protocol.hh>
# include <reactor/scheduler.hh>

namespace reactor
{
  namespace network
  {
    class Server
      : public elle::Printable
    {
    /*------.
    | Types |
    `------*/
    public:
      typedef Server Self;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      /** Create a server.
       *  @param sched The underlying scheduler.
       */
      Server();
      Server(Scheduler& scheduler);
      virtual
      ~Server();

    /*----------.
    | Accepting |
    `----------*/
    public:
      std::unique_ptr<Socket>
      accept();
    protected:
      virtual
      std::unique_ptr<Socket>
      _accept() = 0;
      ELLE_ATTRIBUTE(Scheduler&, scheduler, protected);
    };

    template <typename AsioSocket_, typename EndPoint_, typename Acceptor_>
    class ProtoServer
      : public Server
    {
    /*------.
    | Types |
    `------*/
    public:
      typedef AsioSocket_ AsioSocket;
      typedef Acceptor_ Acceptor;
      typedef EndPoint_ EndPoint;

    /*----------.
    | Accepting |
    `----------*/
    protected:
      using Server::_accept;

    /*----------.
    | Listening |
    `----------*/
    public:
      void
      listen(EndPoint const& end_point);
      EndPoint
      local_endpoint() const;
    protected:
      void
      _accept(AsioSocket& socket, EndPoint& peer);
      ELLE_ATTRIBUTE_X(std::unique_ptr<Acceptor>, acceptor);

    /*----------.
    | Printable |
    `----------*/
    public:
      virtual
      void
      print(std::ostream& stream) const override;
    };
  }
}

#endif
