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
      class SSLCertificate
      {
      public:
        using SSLCertificateMethod = boost::asio::ssl::context::method;

        SSLCertificate(SSLCertificateMethod meth =
                         boost::asio::ssl::context::tlsv1_client);
        SSLCertificate(std::vector<char> const& certificate,
                       std::vector<char> const& key,
                       std::vector<char> const& dh,
                       SSLCertificateMethod meth =
                         boost::asio::ssl::context::tlsv1_server);
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
        using Super = StreamSocket<SSLStream, SSLEndPointType>;
        using Self = SSLSocket;
        using SSLEndPoint = boost::asio::ip::tcp::endpoint;

      public:
        /// Create a client socket.
        SSLSocket(const std::string& hostname,
                  const std::string& port,
                  DurationOpt timeout = DurationOpt());
        /// Create a client socket.
        SSLSocket(SSLEndPoint const& endpoint,
                  DurationOpt timeout = DurationOpt());
        /// Create a server socket.
        SSLSocket(const std::string& hostname,
                  const std::string& port,
                  SSLCertificate& certificate,
                  DurationOpt timeout = DurationOpt());
        /// Create a server socket.
        SSLSocket(SSLEndPoint const& endpoint,
                  SSLCertificate& certificate,
                  DurationOpt timeout = DurationOpt());
        ~SSLSocket();

      /*----------------.
      | Pretty printing |
      `----------------*/
      public:
        void print(std::ostream& s) const;

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
