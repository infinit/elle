#ifndef INFINIT_REACTOR_NETWORK_SERVER_HH
# define INFINIT_REACTOR_NETWORK_SERVER_HH

# include <memory>

# include <reactor/asio.hh>
# include <reactor/network/fwd.hh>
# include <reactor/network/Protocol.hh>
# include <reactor/network/tcp-socket.hh>
# include <reactor/scheduler.hh>

namespace reactor
{
  namespace network
  {
    class Server:
      public elle::Printable
    {
      /*---------.
      | Typedefs |
      `---------*/
      public:
        typedef Server Self;
        typedef boost::asio::ip::tcp::acceptor TCPAcceptor;
        typedef boost::asio::ip::tcp::endpoint EndPoint;

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

        void
        listen(const EndPoint& end_point);

        void
        listen(int port = 0, bool ipv6 = false);

        EndPoint
        local_endpoint() const;

        int
        port() const;

      protected:
        virtual
        std::unique_ptr<Socket>
        _accept() = 0;
        void
        _accept(TCPSocket::AsioSocket& socket, EndPoint& peer);

      protected:
        Scheduler& _scheduler;

      private:
        ELLE_ATTRIBUTE_X(std::unique_ptr<TCPAcceptor>, acceptor);

    /*----------.
    | Printable |
    `----------*/
    public:
      /// Print pretty representation to \a stream.
      virtual
      void
      print(std::ostream& stream) const override;
    };

    template <typename Socket>
    class ProtoServer
    {
      public:
        typedef typename Socket::AsioSocket AsioSocket;
    };
  }
}

#endif
