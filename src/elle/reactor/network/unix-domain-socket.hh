#pragma once

#include <boost/filesystem.hpp>

#include <elle/reactor/network/socket.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      class UnixDomainSocket
        : public StreamSocket<boost::asio::local::stream_protocol::socket,
                              boost::asio::local::stream_protocol::endpoint>
      {
      public:
        using Super = StreamSocket<
          boost::asio::local::stream_protocol::socket,
          boost::asio::local::stream_protocol::endpoint>;
        UnixDomainSocket(EndPoint const& ep,
                         DurationOpt timeout = {});
        UnixDomainSocket(boost::filesystem::path const& path,
                         DurationOpt timeout = {});

      private:
        friend class UnixDomainServer;
        UnixDomainSocket(std::unique_ptr<AsioSocket> socket,
                         AsioSocket::endpoint_type const& endpoint);
      };
    }
  }
}
