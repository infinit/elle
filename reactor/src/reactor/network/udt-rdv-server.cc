#include <algorithm>
#include <random>
#include <chrono>
#include <typeinfo>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

#include <elle/log.hh>
#include <elle/os/environ.hh>
#include <elle/printf.hh>

#include <asio-udt/acceptor.hh>

#include <reactor/exception.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/nat.hh>
#include <reactor/network/buffer.hh>
#include <reactor/network/resolve.hh>
#include <reactor/network/udt-rdv-server.hh>
#include <reactor/operation.hh>
#include <reactor/scheduler.hh>

ELLE_LOG_COMPONENT("reactor.network.UDTRendezVousServer");

namespace reactor
{
  namespace network
  {
    /*-------------.
    | Construction |
    `-------------*/

    UDTRendezVousServer::UDTRendezVousServer(Scheduler& sched)
      : Super(sched)
      , _accepted()
      , _sockets()
      , _udp_socket(new UDPSocket{sched})
    {}

    UDTRendezVousServer::UDTRendezVousServer(Scheduler& sched,
                                             std::unique_ptr<UDPSocket> sock)
      : Super(sched)
      , _accepted()
      , _sockets()
      , _udp_socket(std::move(sock))
    {}

    UDTRendezVousServer::~UDTRendezVousServer()
    {}

    /*----------.
    | Accepting |
    `----------*/

    UDTSocket*
    UDTRendezVousServer::accept()
    {
      if (this->_sockets.empty())
        this->scheduler().current()->wait(_accepted);
      UDTSocket* res = this->_sockets.back().release();
      this->_sockets.pop_back();
      return res;
    }

    void
    UDTRendezVousServer::accept(std::string const& addr, int port)
    {
      ELLE_TRACE("%s: rendezvous connection with %s:%s", this, addr, port);
      this->_sockets.push_back(
        std::unique_ptr<UDTSocket>(
          new UDTSocket(this->scheduler(),
                        this->_udp_socket->socket()->native_handle(),
                        addr, std::to_string(port))));
      ELLE_DEBUG("%s: rendezvous attended", this);
      this->_accepted.signal_one();
    }

    /*----------.
    | Listening |
    `----------*/

    void
    UDTRendezVousServer::listen(int desired_port)
    {
      // FIXME: this code grew this way, but it sucks: we test whether
      // local_endpoint throws, which means the socket is unbound, to bind it
      // for listening. This sucks. Also we should not ignore this call if the
      // socket is bound.
      try
      {
        (void)_udp_socket->local_endpoint();
      }
      catch (...)
      {
        boost::asio::ip::udp::endpoint local(boost::asio::ip::udp::v4(),
                                             desired_port);
        _udp_socket->bind(local);
        ELLE_TRACE("%s: listening on port %s",
                   *this, _udp_socket->local_endpoint());
      }
    }

    UDTRendezVousServer::EndPoint
    UDTRendezVousServer::local_endpoint() const
    {
      return this->_udp_socket->local_endpoint();
    }

    int
    UDTRendezVousServer::port() const
    {
      return local_endpoint().port();
    }

    void
    UDTRendezVousServer::print(std::ostream& s) const
    {
      // Catch std::exception because boost throw some of thoses.
      try
      {
        s << "UDTRendezVousServer " << this->local_endpoint();
      }
      catch (std::exception const& e)
      {
        s << "UDTRendezVousServer [unboud: " << e.what() << "]";
      }
    }
  }
}
