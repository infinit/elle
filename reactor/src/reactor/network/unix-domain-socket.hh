#ifndef REACTOR_NETWORK_UNIX_DOMAIN_SOCKET_HH
# define REACTOR_NETWORK_UNIX_DOMAIN_SOCKET_HH

# include <boost/filesystem.hpp>

# include <reactor/network/socket.hh>

namespace reactor
{
  namespace network
  {
    class UnixDomainSocket
      : public StreamSocket<boost::asio::local::stream_protocol::socket,
                            boost::asio::local::stream_protocol::endpoint>
    {
    public:
      typedef StreamSocket<boost::asio::local::stream_protocol::socket,
                           boost::asio::local::stream_protocol::endpoint> Super;
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


#endif
