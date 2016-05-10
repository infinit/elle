#include <reactor/network/unix-domain-server.hh>

#include <elle/log.hh>

#include <reactor/network/unix-domain-socket.hh>

ELLE_LOG_COMPONENT("reactor.network.UnixDomainSocket");

namespace reactor
{
  namespace network
  {
    UnixDomainServer::UnixDomainServer()
      : Super()
    {}

    std::unique_ptr<UnixDomainSocket>
    UnixDomainServer::accept()
    {
      // Open a new raw socket.
      auto new_socket = elle::make_unique<AsioSocket>(
        reactor::Scheduler::scheduler()->io_service());
      EndPoint peer;
      this->_accept(*new_socket, peer);
      // Socket is now connected so make it into a TCPSocket.
      std::unique_ptr<UnixDomainSocket> res(
        new UnixDomainSocket(std::move(new_socket), peer));
      ELLE_TRACE("%s: got connection: %s", *this, *res);
      return res;
    }

    void
    UnixDomainServer::listen(boost::filesystem::path const& path)
    {
      this->listen(
        boost::asio::local::stream_protocol::endpoint(path.string()));
    }

    UnixDomainServer::EndPoint
    UnixDomainServer::_default_endpoint() const
    {
      if (!this->_tmp)
        this->_tmp.emplace();
      return EndPoint((this->_tmp->path() / "socket").string());
    }

    std::unique_ptr<Socket>
    UnixDomainServer::_accept()
    {
      return this->accept();
    }
  }
}
