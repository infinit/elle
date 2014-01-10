#ifndef INFINIT_REACTOR_NETWORK_SSL_SOCKET_HH
# define INFINIT_REACTOR_NETWORK_SSL_SOCKET_HH

# include <reactor/network/socket.hh>

# include <reactor/scheduler.hh>
# include <reactor/network/resolve.hh>
# include <reactor/network/tcp-socket.hh>

# include <boost/asio.hpp>
# include <boost/asio/ssl.hpp>

namespace reactor
{
  namespace network
  {
    class SSLCertificate
    {
    public:
      typedef boost::asio::ssl::context::method SSLCertificateMethod;

      SSLCertificate(SSLCertificateMethod meth =
                       boost::asio::ssl::context::sslv23);
      SSLCertificate(std::string const& certificate,
                     std::string const& key,
                     std::string const& dhfile,
                     SSLCertificateMethod meth =
                       boost::asio::ssl::context::sslv23);
      SSLCertificate(const SSLCertificate& other);

    private:
      ELLE_ATTRIBUTE_R(boost::asio::ssl::context*, context);
    };

    typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket>
      SSLStream;
    typedef boost::asio::ip::tcp::socket::endpoint_type SSLEndPointType;

    class SSLSocket:
      public StreamSocket <SSLStream, SSLEndPointType>
    {
    public:
      typedef StreamSocket<SSLStream, SSLEndPointType> Super;
      typedef SSLSocket Self;
      typedef SSLStream::handshake_type Handshake_type;
      typedef boost::asio::ip::tcp::endpoint SSLEndPoint;

    public:
      SSLSocket(const std::string& hostname,
                const std::string& port,
                SSLCertificate const& certificate,
                Handshake_type type = Handshake_type::client,
                DurationOpt timeout = DurationOpt());
      SSLSocket(reactor::Scheduler& sched,
                const std::string& hostname,
                const std::string& port,
                SSLCertificate const& certificate,
                Handshake_type type = Handshake_type::client,
                DurationOpt timeout = DurationOpt());
      SSLSocket(reactor::Scheduler& sched,
                SSLEndPoint const& endpoint,
                SSLCertificate const& certificate,
                Handshake_type type = Handshake_type::client,
                DurationOpt timeout = DurationOpt());
      ~SSLSocket();

    /*----------------.
    | Pretty printing |
    `----------------*/
    public:
      void print(std::ostream& s) const;

    private:
      friend class SSLServer;
      SSLSocket(Scheduler& scheduler,
                std::unique_ptr<SSLStream> socket,
                SSLEndPoint const& endpoint,
                SSLCertificate const& certificate);

      /// No check of certificate is done by default
      void
      _handshake();

      void
      _server_handshake();

    private:
      ELLE_ATTRIBUTE(SSLCertificate const&, certificate);
      ELLE_ATTRIBUTE(DurationOpt, timeout);
      ELLE_ATTRIBUTE(Handshake_type, type);
    };
  }
}

#endif // INFINIT_REACTOR_NETWORK_SSL_SOCKET_HH
