#include <boost/optional.hpp>

#include <boost/lexical_cast.hpp>

#include <elle/log.hh>
#include <elle/memory.hh>

#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/resolve.hh>
#include <reactor/network/tcp-socket.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

namespace reactor
{
  namespace network
  {
    /*-------------.
    | Construction |
    `-------------*/

    TCPSocket::TCPSocket(const std::string& hostname,
                         const std::string& port,
                         DurationOpt timeout):
      TCPSocket(resolve_tcp(hostname, port), timeout)
    {}

    TCPSocket::TCPSocket(const std::string& hostname,
                         int port,
                         DurationOpt timeout):
      TCPSocket(hostname, std::to_string(port), timeout)
    {}

    TCPSocket::TCPSocket(boost::asio::ip::tcp::endpoint const& endpoint,
                         DurationOpt timeout):
      Super(elle::make_unique<boost::asio::ip::tcp::socket>(
              reactor::Scheduler::scheduler()->io_service()),
            endpoint, timeout)
    {}

    TCPSocket::TCPSocket(TCPSocket&& socket)
      : Super(std::move(socket))
    {}

    TCPSocket::~TCPSocket()
    {}

    TCPSocket::TCPSocket(std::unique_ptr<AsioSocket> socket,
                         AsioSocket::endpoint_type const& endpoint):
      Super(std::move(socket), endpoint)
    {}
  }
}
