#pragma once

#include <elle/reactor/network/server.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /// A specialized ProtoServer for TCP connections.
      ///
      /// \code{.cc}
      ///
      /// elle::With<elle::reactor::Scope>() << [&](elle::reactor::Scope &s)
      /// {
      ///   elle::reactor::network::TCPServer s();
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
      ///       elle::reactor::network::TCPSocket socket(
      ///         "127.0.0.1",
      ///         elle::sprintf(s.port()),
      ///       std::cout << 'B';
      ///       socket.write(elle::Buffer{"foobar"});
      ///       std::cout << 'C';
      ///     });
      ///   s.wait();
      /// };
      /// // Result: "12AB3C".
      ///
      /// \endcode
      class TCPServer
        : public ProtoServer<boost::asio::ip::tcp::socket,
                             boost::asio::ip::tcp::endpoint,
                             boost::asio::ip::tcp::acceptor>
      {
      /*-------------.
      | Construction |
      `-------------*/
      public:
        using Super = ProtoServer<boost::asio::ip::tcp::socket,
                                  boost::asio::ip::tcp::endpoint,
                                  boost::asio::ip::tcp::acceptor>;
        /// Construct a TCPServer.
        ///
        /// \param no_delay Disable Nagle's algorithm.
        TCPServer(bool no_delay = false);
        virtual
        ~TCPServer();
        /// Get a TCPSocket bound to a peer.
        ///
        /// Wait until one is available.
        std::unique_ptr<TCPSocket>
        accept();

      /*----------.
      | Accepting |
      `----------*/
      public:
        using Super::listen;
        /// Listen on the given port.
        ///
        /// \param port The port to listen to.
        /// \param enable_ipv6 Whether it accepts IPv6 connections.
        void
        listen(int port, bool enable_ipv6=false);
        /// Listen at the given tuple host / port.
        ///
        /// \param host The host.
        /// \param port The port to listen to.
        /// \param enable_ipv6 Whether if it accepts IPv6 connections.
        void
        listen(boost::asio::ip::address host, int port = 0,
               bool enable_ipv6 = false);
        /// Port the TCPServer is listening to.
        int
        port() const;
      protected:
        virtual
        EndPoint
        _default_endpoint() const override;
        using Super::_accept;
        virtual
        std::unique_ptr<Socket>
        _accept() override;
        ELLE_ATTRIBUTE_RX(bool, no_delay);
      };
    }
  }
}
