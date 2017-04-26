#pragma once

#include <elle/reactor/network/socket.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /// A StreamSocket designed for TCP connections.
      class TCPSocket
        : public StreamSocket<boost::asio::ip::tcp::socket>
      {
        /*------.
        | Types |
        `------*/
      public:
        using Super = StreamSocket<boost::asio::ip::tcp::socket>;
        using AsioResolver = boost::asio::ip::tcp::resolver;

        /*-------------.
        | Construction |
        `-------------*/
      public:
        /// Construct a TCPSocket.
        ///
        /// \param hostname The name of the host.
        /// \param port The port the host is listening to.
        /// \param timeout The maximum duration before the connection attempt
        ///                times out.
        TCPSocket(const std::string& hostname,
                  const std::string& port,
                  DurationOpt timeout = DurationOpt());
        /// @see TCPSocket::TCPSocket.
        TCPSocket(const std::string& hostname,
                  int port,
                  DurationOpt timeout = DurationOpt());
        /// Construct a TCPSocket.
        ///
        /// \param endpoint The EndPoint of the host.
        /// \param timeout The maximum duration before the connection attempt
        ///                times out.
        TCPSocket(boost::asio::ip::tcp::endpoint const& endpoint,
                  DurationOpt timeout = DurationOpt());
        TCPSocket(TCPSocket&& src);
        ~TCPSocket() override;
      private:
        friend class TCPServer;
        TCPSocket(std::unique_ptr<AsioSocket> socket,
                  AsioSocket::endpoint_type const& endpoint);
      };
    }
  }
}
