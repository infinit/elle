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
#include <reactor/network/resolve.hh>
#include <reactor/network/udt-server.hh>
#include <reactor/operation.hh>
#include <reactor/scheduler.hh>

#include <elle/printf.hh>
#include <elle/log.hh>
#include <reactor/network/nat.hh>
#include <elle/os/environ.hh>

//ELLE_LOG_COMPONENT("reactor.network.UDTServer");

namespace reactor
{
  namespace network
  {
    /*----------.
    | Accepting |
    `----------*/

    class UDTAccept
      : public Operation
    {
    public:
      UDTAccept(Scheduler& scheduler,
                boost::asio::ip::udt::acceptor& acceptor)
        : Operation(scheduler)
        , _acceptor(acceptor)
        , _socket(nullptr)
      {}

      virtual
      const char*
      type_name() const
      {
        static const char* name = "server accept";
        return name;
      }

      UDTSocket::AsioSocket*
      socket()
      {
        return _socket;
      }

    protected:
      virtual
      void
      _abort()
      {
        _acceptor.cancel();
        _signal();
      }

      virtual
      void
      _start()
      {
        _acceptor.async_accept(boost::bind(&UDTAccept::_wakeup,
                                           this, _1, _2));
      }

    private:

      void
      _wakeup(boost::system::error_code const& error,
            boost::asio::ip::udt::socket* socket)

      {
        if (error == boost::system::errc::operation_canceled)
          return;
        if (error)
          _raise<Exception>(error.message());
        _socket = socket;
        _signal();
      }

      ELLE_ATTRIBUTE(boost::asio::ip::udt::acceptor&, acceptor);
      ELLE_ATTRIBUTE(UDTSocket::AsioSocket*, socket);
    };

    /*-------------.
    | Construction |
    `-------------*/

    UDTServer::UDTServer(Scheduler& sched):
      Super(sched),
      _acceptor(nullptr)
    {}

    UDTServer::~UDTServer()
    {}

    UDTSocket*
    UDTServer::accept()
    {
      auto& sched = *reactor::Scheduler::scheduler();

      UDTAccept action{sched, *this->_acceptor};
      action.run();
      return new UDTSocket{sched, action.socket()};
    }

    /*----------.
    | Listening |
    `----------*/

    void
    UDTServer::listen(int port)
    {
      namespace udt = boost::asio::ip::udt;
      auto& sched = *reactor::Scheduler::scheduler();

      this->_acceptor.reset(new udt::acceptor{sched.io_service(), port});
    }

    UDTServer::EndPoint
    UDTServer::local_endpoint() const
    {
      // XXX We have to do the matching function in asio-udt.
      return {
        boost::asio::ip::udp::v4(),
        static_cast<unsigned short>(this->port()),
      };
    }

    int
    UDTServer::port() const
    {
      return this->_acceptor->port();
    }

    void
    UDTServer::print(std::ostream& s) const
    {
      s << "UDTServer " << this->local_endpoint();
    }
  }
}
