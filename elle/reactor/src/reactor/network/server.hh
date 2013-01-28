#ifndef INFINIT_REACTOR_NETWORK_SERVER_HH
# define INFINIT_REACTOR_NETWORK_SERVER_HH

# include <memory>

# include <boost/asio.hpp>

# include <reactor/network/fwd.hh>
# include <reactor/network/Protocol.hh>

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
         *  @param sched The underlying scheduler.
         */
        Server(Scheduler& sched);
        virtual ~Server();
        /** Create a server for the given protocol.
         *  @param protocol The transport protocl to use.
         *  @param sched The underlying scheduler.
         */
        static
        std::unique_ptr<Server>
        create(Protocol protocol, Scheduler& sched);

      /*----------.
      | Accepting |
      `----------*/
      public:
        virtual void listen(int port) = 0;
        virtual Socket* accept() = 0;
        virtual void accept(std::string const& addr, int port);
        virtual int port() const = 0;

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
