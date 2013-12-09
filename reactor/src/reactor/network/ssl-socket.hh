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
      typedef boost::asio::ssl::stream<
        boost::asio::ip::tcp::socket>::handshake_type Handshake_type;

    public:
      SSLSocket(const std::string& hostname,
                const std::string& port,
                SSLCertif& cert,
                Handshake_type type = Handshake_type::client,
                DurationOpt timeout = DurationOpt());
      SSLSocket(reactor::Scheduler& sched,
                const std::string& hostname,
                const std::string& port,
                SSLCertif& cert,
                Handshake_type type = Handshake_type::client,
                DurationOpt timeout = DurationOpt());
      SSLSocket(reactor::Scheduler& sched,
                boost::asio::ip::tcp::endpoint const& endpoint,
                SSLCertif& cert,
                Handshake_type type,
                DurationOpt timeout);
      SSLSocket(reactor::Scheduler& sched,
                boost::asio::ip::tcp::endpoint const& endpoint,
                SSLCertif& cert,
                Handshake_type type);
      ~SSLSocket();

    private:
      friend class SSLServer;

      bool
      _handshake();

      void
      _server_handshake();

    private:
      ELLE_ATTRIBUTE(SSLCertif, cert);
      ELLE_ATTRIBUTE(Handshake_type, type);
    };
  }
}

#endif // !INFINIT_REACTOR_NETWORK_SSL_SOCKET_HH
