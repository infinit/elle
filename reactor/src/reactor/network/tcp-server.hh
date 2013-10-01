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
        TCPServer();
        TCPServer(Scheduler& sched);
        virtual ~TCPServer();

      /*----------.
      | Listening |
      `----------*/
      public:
        void listen(int port = 0);
        void listen(const EndPoint& end_point);

      public:
        /// The locally bound port.
        virtual int port() const;
        /// The locally bound endpoint.
        EndPoint local_endpoint() const;

      /*----------.
      | Accepting |
      `----------*/
      public:
        TCPSocket* accept();

      private:
        std::unique_ptr<boost::asio::ip::tcp::acceptor> _acceptor;
    };
  }
}

#endif
