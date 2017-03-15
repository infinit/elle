#pragma once

#include <boost/filesystem.hpp>

#include <elle/reactor/network/socket.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /// Specialized StreamSocket for inter-process communication.
      class UnixDomainSocket
        : public StreamSocket<boost::asio::local::stream_protocol::socket,
                              boost::asio::local::stream_protocol::endpoint>
      {
      public:
        using Super = StreamSocket<
          boost::asio::local::stream_protocol::socket,
          boost::asio::local::stream_protocol::endpoint>;
        /// Construct a UnixDomainSocket.
        ///
        /// \param ep The EndPoint of the peer.
        /// \param timeout The  maximum duration before the connection attempt
        ///                times out.
        UnixDomainSocket(EndPoint const& ep,
                         DurationOpt timeout = {});
        /// Construct a UnixDomainSocket.
        ///
        /// \param path The path name to construct an endpoint from.
        /// \param timeout The maximum duration before the connection attempt
        ///                times out.
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
