#include <elle/reactor/network/unix-domain-socket.hh>
#include <elle/reactor/scheduler.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      UnixDomainSocket::UnixDomainSocket(EndPoint const& ep,
                                         DurationOpt timeout)
        : Super(
          std::make_unique<boost::asio::local::stream_protocol::socket>(
            reactor::scheduler().io_service()),
          ep, std::move(timeout))
      {}

      UnixDomainSocket::UnixDomainSocket(boost::filesystem::path const& path,
                                         DurationOpt timeout)
        : UnixDomainSocket(EndPoint(path.string()), std::move(timeout))
      {}

      UnixDomainSocket::UnixDomainSocket(
        std::unique_ptr<AsioSocket> socket,
        AsioSocket::endpoint_type const& endpoint)
        : Super(std::move(socket), endpoint)
      {}
    }
  }
}
