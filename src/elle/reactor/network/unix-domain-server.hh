#pragma once

#include <boost/filesystem.hpp>

#include <elle/filesystem/TemporaryDirectory.hh>
#include <elle/reactor/network/server.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      class UnixDomainServer
        : public ProtoServer<boost::asio::local::stream_protocol::socket,
                             boost::asio::local::stream_protocol::endpoint,
                             boost::asio::local::stream_protocol::acceptor>
      {
      public:
        typedef ProtoServer<boost::asio::local::stream_protocol::socket,
                            boost::asio::local::stream_protocol::endpoint,
                            boost::asio::local::stream_protocol::acceptor> Super;
        UnixDomainServer();
        ~UnixDomainServer();
        std::unique_ptr<UnixDomainSocket>
        accept();
        using Super::listen;
        void
        listen(boost::filesystem::path const& path);

      protected:
        virtual
        EndPoint
        _default_endpoint() const override;
        using Super::_accept;
        std::unique_ptr<Socket>
        _accept() override;
        ELLE_ATTRIBUTE(
          boost::optional<elle::filesystem::TemporaryDirectory>, tmp, mutable);
        ELLE_ATTRIBUTE(boost::filesystem::path, endpoint_path);
      };
    }
  }
}
