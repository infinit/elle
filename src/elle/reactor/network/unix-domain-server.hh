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
      /// Specialized ProtoServer providing for UnixDomainSocket (inter-process
      /// communication socket).
      ///
      /// \code{.cc}
      ///
      /// auto us = std::make_unique<elle::reactor::network::UnixDomainServer>();
      /// us->listen(this->_socket_path);
      /// this->_server =
      ///   std::make_unique<elle::reactor::network::HttpServer>(std::move(us));
      ///
      /// \endcode
      class UnixDomainServer
        : public ProtoServer<boost::asio::local::stream_protocol::socket,
                             boost::asio::local::stream_protocol::endpoint,
                             boost::asio::local::stream_protocol::acceptor>
      {
      public:
        using Super = ProtoServer<
          boost::asio::local::stream_protocol::socket,
          boost::asio::local::stream_protocol::endpoint,
          boost::asio::local::stream_protocol::acceptor>;
        UnixDomainServer();
        ~UnixDomainServer();
        /// @see Server::accept.
        std::unique_ptr<UnixDomainSocket>
        accept();
        using Super::listen;
        /// @see Server::listen.
        void
        listen(boost::filesystem::path const& path);

      protected:
        /// @see Server::_default_endpoint.
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
