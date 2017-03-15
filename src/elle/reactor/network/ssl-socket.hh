#pragma once

#include <elle/reactor/network/socket.hh>
#include <elle/reactor/network/resolve.hh>
#include <elle/reactor/network/tcp-socket.hh>

// Do not include asio first, this will result on errors on mingw (at least),
// related to WinSock.h already included.
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /// An SSL context.
      ///
      /// SSLCertificate is just an helper for boost::asio::ssl::context.
      class SSLCertificate
      {
      public:
        using SSLCertificateMethod = boost::asio::ssl::context::method;

        /// Create an SSLCertificate.
        ///
        /// Initialize the underlying context with the given method and set
        /// verify to none.
        /// This is the client implementation of the SSLCertificate.
        ///
        /// \param meth The ssl::context::method to use.
        SSLCertificate(SSLCertificateMethod meth =
                       boost::asio::ssl::context::tlsv1_client);
        /// Create an SSLCertificate from given certificate, key and dh.
        ///
        /// \param certificate The content of the certificate.
        /// \param key The key.
        /// \param dh The Diffie-Hellman key exchange.
        /// \param meth The ssl::context::method to use.
        SSLCertificate(std::vector<char> const& certificate,
                       std::vector<char> const& key,
                       std::vector<char> const& dh,
                       SSLCertificateMethod meth =
                         boost::asio::ssl::context::tlsv1_server);
        /// Create an SSLCertificate from given paths for certificate, key and
        /// dh.
        ///
        /// \param certificate The path to the certificate.
        /// \param key The path to the key.
        /// \param dh The path to the Diffie-Hellman key exchange.
        /// \param meth The ssl::context::method to use.
        SSLCertificate(std::string const& certificate,
                       std::string const& key,
                       std::string const& dhfile,
                       SSLCertificateMethod meth =
                         boost::asio::ssl::context::tlsv1_server);

      private:
        ELLE_ATTRIBUTE_RX(boost::asio::ssl::context, context);
      };

      using SSLStream = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
      using SSLEndPointType = boost::asio::ip::tcp::socket::endpoint_type;

      /// XXX[doc]
      class SSLCertificateOwner
      {
      public:
        SSLCertificateOwner(
          std::shared_ptr<SSLCertificate> certificate = nullptr);

        ~SSLCertificateOwner() = default;

      private:
        ELLE_ATTRIBUTE_R(std::shared_ptr<SSLCertificate>, certificate);
      };

      /// An StreamSocket designed for SSL connections.
      class SSLSocket
        : public SSLCertificateOwner
        , public StreamSocket <SSLStream, SSLEndPointType>
      {
      public:
        using Super = StreamSocket<SSLStream, SSLEndPointType>;
        using Self = SSLSocket;
        using SSLEndPoint = boost::asio::ip::tcp::endpoint;

      public:
        /// Construct a client socket.
        ///
        /// \param hostname The name of the host.
        /// \param port The port the host is listening to.
        /// \param timeout The maximum duration before the connection attempt
        ///                times out.
        SSLSocket(const std::string& hostname,
                  const std::string& port,
                  DurationOpt timeout = DurationOpt());
        /// Construct a client socket.
        ///
        /// \param endpoint The EndPoint of the host.
        /// \param timeout The maximum duration before the connection attempt
        ///                times out.
        SSLSocket(SSLEndPoint const& endpoint,
                  DurationOpt timeout = DurationOpt());
        /// Construct a server socket.
        ///
        /// \param hostname The name of the host.
        /// \param port The port the host is listening to.
        /// \param certificate An SSLCertificate.
        /// \param timeout The maximum duration before the connection attempt
        ///                times out.
        SSLSocket(const std::string& hostname,
                  const std::string& port,
                  SSLCertificate& certificate,
                  DurationOpt timeout = DurationOpt());
        /// Construct a server socket.
        ///
        /// \param endpoint The EndPoint of the host.
        /// \param certificate An SSLCertificate.
        /// \param timeout The maximum duration before the connection attempt
        ///                times out.
        SSLSocket(SSLEndPoint const& endpoint,
                  SSLCertificate& certificate,
                  DurationOpt timeout = DurationOpt());
        ~SSLSocket();

      /*----------------.
      | Pretty printing |
      `----------------*/
      public:
        void
        print(std::ostream& s) const;

      /*-----------.
      | Connection |
      `-----------*/
      private:
        friend class SSLServer;
        SSLSocket(std::unique_ptr<SSLStream> socket,
                  SSLEndPoint const& endpoint,
                  std::shared_ptr<SSLCertificate> certificate,
                  DurationOpt timeout = DurationOpt());
        /// No check of certificate is done by default
        void
        _client_handshake();
        void
        _server_handshake(reactor::DurationOpt const& timeout);
        void
        _shutdown();

      private:
        ELLE_ATTRIBUTE_RW(bool, shutdown_asynchronous);
        ELLE_ATTRIBUTE(DurationOpt, timeout);
      };
    }
  }
}
