#ifndef INFINIT_REACTOR_NETWORK_TCP_SERVER_HH
# define INFINIT_REACTOR_NETWORK_TCP_SERVER_HH

# include <reactor/network/server.hh>
# include <reactor/scheduler.hh>

namespace reactor
{
  namespace network
  {
    class TCPServer:
      public Server,
      public ProtoServer<TCPSocket>
    {
      /*-------------.
      | Construction |
      `-------------*/
      public:
        typedef Server Super;
        TCPServer();
        TCPServer(Scheduler& scheduler);

        virtual
        ~TCPServer();

    /*----------.
    | Accepting |
    `----------*/
    public:
      std::unique_ptr<TCPSocket>
      accept();
    };
  }
}

#endif // INFINIT_REACTOR_NETWORK_TCP_SERVER_HH
