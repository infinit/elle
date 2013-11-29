#ifndef INFINIT_REACTOR_NETWORK_SSL_SERVER_HH
# define INFINIT_REACTOR_NETWORK_SSL_SERVER_HH

# include <reactor/scheduler.hh>

# include <reactor/network/server.hh>
# include <reactor/network/ssl-socket.hh>
# include <reactor/exception.hh>

namespace reactor
{
  namespace network
  {
    class SSLServer : public Server, ProtoServer<SSLSocket>
    {
    public:
      typedef Server Super;

    public:
      SSLServer(SSLCertif& cert);
      SSLServer(reactor::Scheduler& sched, SSLCertif& cert);
      virtual ~SSLServer();

    /*----------.
    | Listening |
    `----------*/
    public:
      void listen(int port = 0);
      void listen(const EndPoint& end_point);

    public:
      virtual int port() const;
      EndPoint local_endpoint() const;

    /*----------.
    | Accepting |
    `----------*/
    public:
      SSLSocket* accept();

    private:
      std::unique_ptr<boost::asio::ip::tcp::acceptor> _acceptor;

    /*------------.
    | Certificate |
    `------------*/
    private:
      ELLE_ATTRIBUTE(SSLCertif, cert);
    };
  }
}

#endif // !INFINIT_REACTOR_NETWORK_SSL_SERVER_HH
