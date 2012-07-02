#ifndef INFINIT_REACTOR_NETWORK_TCP_SERVER_HH
# define INFINIT_REACTOR_NETWORK_TCP_SERVER_HH

# include <reactor/network/server.hh>
# include <reactor/network/tcp-socket.hh>

namespace reactor
{
  namespace network
  {
    class TCPServer: public Server, public ProtoServer<TCPSocket>
    {
      public:
        typedef Server Super;
        TCPServer(Scheduler& sched);
        virtual ~TCPServer();

      /*----------.
      | Listening |
      `----------*/
      public:
        void listen(int port);
        void listen(const EndPoint& end_point);

      public:
        /// Retreive the locally bound endpoint
        EndPoint local_endpoint() const;

      /*----------.
      | Accepting |
      `----------*/
      public:
        TCPSocket* accept();

      private:
        boost::asio::ip::tcp::acceptor* _acceptor;
    };
  }
}

#endif
