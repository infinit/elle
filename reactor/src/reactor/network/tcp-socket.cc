#include <boost/optional.hpp>

#include <boost/lexical_cast.hpp>

#include <elle/log.hh>

#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/resolve.hh>
#include <reactor/network/tcp-socket.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

ELLE_LOG_COMPONENT("reactor.network.TCPSocket");

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
      TCPSocket(*reactor::Scheduler::scheduler(), hostname, port, timeout)
    {}

    TCPSocket::TCPSocket(Scheduler& sched,
                         const std::string& hostname,
                         const std::string& port,
                         DurationOpt timeout):
      TCPSocket(sched, resolve_tcp(sched, hostname, port), timeout)
    {}

    TCPSocket::TCPSocket(const std::string& hostname,
                         int port,
                         DurationOpt timeout):
      TCPSocket(*reactor::Scheduler::scheduler(), hostname, port, timeout)
    {}

    TCPSocket::TCPSocket(Scheduler& sched,
                         const std::string& hostname,
                         int port,
                         DurationOpt timeout)
      : TCPSocket(sched, resolve_tcp(sched,
                                     hostname,
                                     boost::lexical_cast<std::string>(port)),
                  timeout)
    {}

    TCPSocket::TCPSocket(Scheduler& sched,
                         boost::asio::ip::tcp::endpoint const& endpoint,
                         DurationOpt timeout):
      Super(sched, endpoint, timeout)
    {}

    TCPSocket::~TCPSocket()
    {
      ELLE_TRACE_SCOPE("%s: finalize", *this);
      // XXX: Flush the socket, otherwise the parent ~IOStream will flush the
      // buffer which will in turn write to the (deleted) socket.
      if (!this->fail() && !this->bad())
        try
        {
          this->flush();
        }
        catch (...)
        {
          _pacify_streambuffer();
          // XXX: std::iostream technically can't throw due to virtual noexcept
          // destructor. Find something better.
          // throw;
          ELLE_ERR("error while flushing socket: %s", elle::exception_string());
        }
    }

    TCPSocket::TCPSocket(Scheduler& sched,
                         AsioSocket* socket,
                         AsioSocket::endpoint_type const& peer):
      Super(sched, socket, peer)
    {}

    /*----------------.
    | Pretty Printing |
    `----------------*/

    void
    TCPSocket::print(std::ostream& s) const
    {
      s << "TCPSocket(" << peer() << ")";
    }
  }
}
