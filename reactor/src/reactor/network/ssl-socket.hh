#ifndef INFINIT_REACTOR_NETWORK_SSL_SOCKET_HH
# define INFINIT_REACTOR_NETWORK_SSL_SOCKET_HH

# include <reactor/network/socket.hh>

# include <reactor/scheduler.hh>
# include <reactor/network/resolve.hh>

# include <boost/asio.hpp>
# include <boost/asio/ssl.hpp>

namespace reactor
{
  namespace network
  {
    class SSLCertif
    {
    public:
      SSLCertif(boost::asio::ssl::context::method meth =
                  boost::asio::ssl::context::sslv23);
      SSLCertif(std::string const& cert,
                std::string const& key,
                std::string const& dhfile,
                boost::asio::ssl::context::method meth =
                  boost::asio::ssl::context::sslv23);
      SSLCertif(const SSLCertif& other);

      void
      release();

    private:
      ELLE_ATTRIBUTE_R(boost::asio::ssl::context*, ctx);
    };

    class SSLSocket: public StreamSocket<
      boost::asio::ssl::stream<boost::asio::ip::tcp::socket>,
      boost::asio::ip::tcp::socket::endpoint_type>
    {
    public:
      typedef boost::asio::ssl::context SSLContext;
      typedef StreamSocket<boost::asio::ssl::stream<
        boost::asio::ip::tcp::socket>,
        boost::asio::ip::tcp::socket::endpoint_type> Super;
      typedef SSLSocket Self;

    public:
      SSLSocket(const std::string& hostname,
                const std::string& port,
                SSLCertif& cert,
                DurationOpt timeout = DurationOpt());
      SSLSocket(reactor::Scheduler& sched,
                const std::string& hostname,
                const std::string& port,
                SSLCertif& cert,
                DurationOpt timeout = DurationOpt());
      SSLSocket(reactor::Scheduler& sched,
                boost::asio::ip::tcp::endpoint const& endpoint,
                SSLCertif& cert,
                DurationOpt timeout = DurationOpt());
      ~SSLSocket();

      bool
      handshake();

      void
      server_handshake();

    private:
      SSLSocket(Scheduler& sched,
                std::unique_ptr<boost::asio::ssl::stream<
                  boost::asio::ip::tcp::socket>> socket,
                boost::asio::ip::tcp::socket::endpoint_type const& peer);

    private:
      boost::asio::ip::tcp::socket&
      asio_socket();

      friend class SSLServer;
      friend class SSLAccept;

    private:
      ELLE_ATTRIBUTE(SSLCertif, cert);
    };
  }
}

#endif // !INFINIT_REACTOR_NETWORK_SSL_SOCKET_HH
