#ifndef INFINIT_REACTOR_NETWORK_UDT_SERVER_HH
# define INFINIT_REACTOR_NETWORK_UDT_SERVER_HH

# include <reactor/network/server.hh>
# include <reactor/network/udt-socket.hh>

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

      public:
        /// Retrieve the locally bound endpoint
        EndPoint local_endpoint() const;

      /*----------.
      | Accepting |
      `----------*/
      public:
        UDTSocket* accept();

      private:
        boost::asio::ip::udt::acceptor* _acceptor;
    };
  }
}

#endif
