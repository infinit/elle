#pragma once

#include <elle/reactor/network/socket.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      class TCPSocket
        : public StreamSocket<boost::asio::ip::tcp::socket>
      {
        /*---------.
          | Typedefs |
          `---------*/
      public:
        using Super = StreamSocket<boost::asio::ip::tcp::socket>;
        using AsioResolver = boost::asio::ip::tcp::resolver;

        /*-------------.
          | Construction |
          `-------------*/
      public:
        TCPSocket(const std::string& hostname,
                  const std::string& port,
                  DurationOpt timeout = DurationOpt());
        TCPSocket(const std::string& hostname,
                  int port,
                  DurationOpt timeout = DurationOpt());
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
