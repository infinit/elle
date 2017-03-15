#pragma once

#include <elle/reactor/Channel.hh>
#include <elle/reactor/network/server.hh>
#include <elle/reactor/network/ssl-socket.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /// A specialized ProtoServer for TCP SSL connections.
      ///
      /// This performs a handshake with the Sockets attempting to connect,
      /// using the given certificate.
      ///
      /// \code{.cc}
      ///
      /// // Consider an SSLCertificate certificate.
      /// elle::With<elle::reactor::Scope>() << [&](elle::reactor::Scope &s)
      /// {
      ///   elle::reactor::network::SSLServer s(certificate, 10_sec);
      ///   elle::reactor::Barrier listening;
      ///   s.run_background("server",
      ///     [&]
      ///     {
      ///       s.listen();
      ///       std::cout << '1';
      ///       listening.open();
      ///       std::cout << '2';
      ///       auto socket = s.accept();
      ///       std::cout << '3';
      ///     });
      ///   s.run_background("client",
      ///     [&]
      ///     {
      ///       listening.wait();
      ///       std::cout << 'A';
      ///       elle::reactor::network::SSLSocket socket(
      ///         "127.0.0.1",
      ///         elle::sprintf(s.port()),
      ///         certificate);
      ///       std::cout << 'B';
      ///       socket.write(elle::Buffer{"foobar"});
      ///       std::cout << 'C';
      ///     });
      ///   s.wait();
      /// };
      /// // Result: "12AB3C".
      ///
      /// \endcode
      class SSLServer
        : public ProtoServer<boost::asio::ip::tcp::socket,
                             boost::asio::ip::tcp::endpoint,
                             boost::asio::ip::tcp::acceptor>
      {
      /*------.
      | Types |
      `------*/
      public:
        using Super = ProtoServer<boost::asio::ip::tcp::socket,
                                  boost::asio::ip::tcp::endpoint,
                                  boost::asio::ip::tcp::acceptor>;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        /// Create an SSLServer with the given certificate.
        ///
        /// \param certificate An SSLCertificate to check socket authenticity.
        /// \param handshake_timeout The maximum duration before the handshake
        ///                          times out.
        SSLServer(std::unique_ptr<SSLCertificate> certificate,
                  reactor::Duration  handshake_timeout = 30_sec);
        virtual
        ~SSLServer();

      /*----------.
      | Accepting |
      `----------*/
      public:
        /// Get a Socket to a peer. Wait until a peer is available.
        std::unique_ptr<SSLSocket>
        accept();
        using Super::listen;
        /// Listen on the given port.
        ///
        /// \param port Port to listen to.
        void
        listen(int port);
        /// Port the SSLServer is listening to.
        int
        port() const;
      protected:
        using Super::_accept;
        virtual
        std::unique_ptr<Socket>
        _accept() override;
        virtual
        EndPoint
        _default_endpoint() const override;
      private:
        void
        _handshake();
        ELLE_ATTRIBUTE(std::shared_ptr<SSLCertificate>, certificate);
        ELLE_ATTRIBUTE(reactor::Duration, handshake_timeout);
        ELLE_ATTRIBUTE(reactor::Channel<std::unique_ptr<SSLSocket>>, sockets);
        ELLE_ATTRIBUTE(reactor::Thread, handshake_thread);
        ELLE_ATTRIBUTE_RW(bool, shutdown_asynchronous);
      };
    }
  }
}
