#include <elle/log.hh>
#include <elle/reactor/network/Error.hh>
#include <elle/reactor/network/unix-domain-server.hh>
#include <elle/reactor/network/unix-domain-socket.hh>

ELLE_LOG_COMPONENT("elle.reactor.network.UnixDomainSocket");

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      UnixDomainServer::UnixDomainServer()
        : Super()
      {}

      UnixDomainServer::~UnixDomainServer()
      {
        if (boost::filesystem::exists(this->_endpoint_path))
          boost::filesystem::remove(this->_endpoint_path);
      }

      std::unique_ptr<UnixDomainSocket>
      UnixDomainServer::accept()
      {
        // Open a new raw socket.
        auto new_socket = std::make_unique<AsioSocket>(
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
        try
        {
          this->listen(
            boost::asio::local::stream_protocol::endpoint(path.string()));
          this->_endpoint_path = path;
        }
        catch (boost::system::system_error& e)
        {
          auto message =
            elle::sprintf("unable to listen on %s: %s", path, e.what());
          if (e.code() == boost::asio::error::name_too_long)
            throw InvalidEndpoint(message);
          else
            throw Error(message);
        }
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
}
