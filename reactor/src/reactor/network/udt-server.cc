#include <algorithm>
#include <random>
#include <chrono>
#include <typeinfo>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

#include <asio-udt/acceptor.hh>

#include <reactor/exception.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/buffer.hh>
#include <reactor/network/resolve.hh>
#include <reactor/network/udt-server.hh>
#include <reactor/operation.hh>
#include <reactor/scheduler.hh>

#include <elle/printf.hh>
#include <elle/log.hh>
#include <reactor/network/nat.hh>
#include <elle/os/getenv.hh>

ELLE_LOG_COMPONENT("reactor.network.UDTServer");

namespace reactor
{
  namespace network
  {

    /*-------------.
    | Construction |
    `-------------*/

    UDTServer::UDTServer(Scheduler& sched):
      Super(sched),
      _accepted(),
      _sockets(),
      _udp_socket(new UDPSocket{sched})
    {
      ELLE_TRACE_METHOD(sched);
    }

    UDTServer::UDTServer(Scheduler& sched,
                         std::unique_ptr<UDPSocket> sock):
      Super(sched),
      _accepted(),
      _sockets(),
      _udp_socket(std::move(sock))
    {
      ELLE_TRACE_METHOD(sched, sock);
    }

    UDTServer::~UDTServer()
    {
    }

    /*----------.
    | Accepting |
    `----------*/

    class UDTAccept: public Operation
    {
      public:
        UDTAccept(Scheduler& scheduler,
                  boost::asio::ip::udt::acceptor& acceptor):
          Operation(scheduler),
          _acceptor(acceptor),
          _socket(0)
        {}

        virtual const char* type_name() const
        {
          static const char* name = "server accept";
          return name;
        }

        UDTSocket::AsioSocket* socket()
        {
          return _socket;
        }

      protected:
        virtual void _abort()
        {
          _acceptor.cancel();
          _signal();
        }

        virtual void _start()
        {
          _socket = new UDTSocket::AsioSocket(scheduler().io_service());
          _acceptor.async_accept(boost::bind(&UDTAccept::_wakeup,
                                             this, _1, _2));
        }

      private:

      void _wakeup(boost::system::error_code const& error,
                   boost::asio::ip::udt::socket* socket)

        {
          if (error == boost::system::errc::operation_canceled)
            return;
          if (error)
            _raise<Exception>(error.message());
          _socket = socket;
          _signal();
        }

        boost::asio::ip::udt::acceptor& _acceptor;
        UDTSocket::AsioSocket* _socket;
    };

    UDTSocket*
    UDTServer::accept()
    {
      if (this->_sockets.empty())
        this->scheduler().current()->wait(_accepted);
      UDTSocket* res = this->_sockets.back().release();
      this->_sockets.pop_back();
      return res;
    }

    void
    UDTServer::accept(std::string const& addr, int port)
    {
      ELLE_TRACE("%s: rendezvous connection with %s:%s", this, addr, port);
      this->_sockets.push_back
        (std::unique_ptr<UDTSocket>
         (new UDTSocket(this->scheduler(),
                        this->_udp_socket->socket()->native_handle(),
                        addr,
                        boost::lexical_cast<std::string>(port))));
      ELLE_DEBUG("%s: rendezvous attended", this);
      this->_accepted.signal_one();
    }

    /*----------.
    | Listening |
    `----------*/

    void
    UDTServer::listen(int desired_port)
    {
      // FIXME: this code grew this way, but it sucks: we test whether
      // local_endpoint throws, which means the socket is unbound, to bind it
      // for listening. This sucks. Also we should not ignore this call if the
      // socket is bound.
      try
      {
        auto endpoint = _udp_socket->local_endpoint();
      }
      catch (...)
      {
        boost::asio::ip::udp::endpoint local(boost::asio::ip::udp::v4(),
                                             desired_port);
        _udp_socket->bind(local);
      }
    }

    UDTServer::EndPoint
    UDTServer::local_endpoint() const
    {
      return this->_udp_socket->local_endpoint();
    }

    int
    UDTServer::port() const
    {
      return local_endpoint().port();
    }

    void
    UDTServer::print(std::ostream& s) const
    {
      s << "UDTServer " << this->local_endpoint();
    }

  }
}
