#ifndef REACTOR_NETWORK_UNIX_DOMAIN_SERVER_HH
# define REACTOR_NETWORK_UNIX_DOMAIN_SERVER_HH

# include <boost/filesystem.hpp>

# include <elle/filesystem/TemporaryDirectory.hh>

# include <reactor/network/server.hh>

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
      _accept();
      ELLE_ATTRIBUTE(
        boost::optional<elle::filesystem::TemporaryDirectory>, tmp, mutable);
    };
  }
}


#endif
