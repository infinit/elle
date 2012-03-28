#ifndef INFINIT_REACTOR_NETWORK_SERVER_HH
# define INFINIT_REACTOR_NETWORK_SERVER_HH

# include <reactor/asio.hh>

# include <reactor/network/fwd.hh>
# include <reactor/network/socket.hh>

namespace reactor
{
  namespace network
  {
    class Server
    {
      /*---------.
      | Typedefs |
      `---------*/
      public:
        typedef Server Self;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        /** Create a server.
         *  @param service The underlying boost io service.
         */
        Server(Scheduler& sched);
        ~Server();

      /*-----------.
      | Scheduling |
      `-----------*/
      public:
        Scheduler& scheduler();
      private:
        Scheduler& _sched;
    };

    template <typename Socket>
    class ProtoServer
    {
      public:
        typedef typename Socket::AsioSocket AsioSocket;
        typedef typename Socket::EndPoint EndPoint;
    };
  }
}

#endif
