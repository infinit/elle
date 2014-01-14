#ifndef INFINIT_REACTOR_NETWORK_SSL_SOCKET_HH
# define INFINIT_REACTOR_NETWORK_SSL_SOCKET_HH

# include <reactor/network/socket.hh>

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

    class SSLCertificateOwner
    {
    public:
      SSLCertificateOwner(
        std::shared_ptr<SSLCertificate> certificate = nullptr);

      ~SSLCertificateOwner() = default;

    private:
      ELLE_ATTRIBUTE_R(std::shared_ptr<SSLCertificate>, certificate);
    };

    class SSLSocket:
      public SSLCertificateOwner,
      public StreamSocket <SSLStream, SSLEndPointType>
    {
    public:
      typedef StreamSocket<SSLStream, SSLEndPointType> Super;
      typedef SSLSocket Self;
      typedef boost::asio::ip::tcp::endpoint SSLEndPoint;

    public:
      SSLSocket(const std::string& hostname,
                const std::string& port,
                DurationOpt timeout = DurationOpt());
      SSLSocket(SSLEndPoint const& endpoint,
                DurationOpt timeout = DurationOpt());
      ~SSLSocket();

    /*----------------.
    | Pretty printing |
    `----------------*/
    public:
      void print(std::ostream& s) const;

    private:
      friend class SSLServer;
      SSLSocket(std::unique_ptr<SSLStream> socket,
                SSLEndPoint const& endpoint,
                std::shared_ptr<SSLCertificate> certificate);

      /// No check of certificate is done by default
      void
      _client_handshake();

      void
      _server_handshake();

    private:
      ELLE_ATTRIBUTE(DurationOpt, timeout);
    };
  }
}

#endif // INFINIT_REACTOR_NETWORK_SSL_SOCKET_HH
