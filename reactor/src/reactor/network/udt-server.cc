#include <algorithm>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

#include <asio-udt/acceptor.hh>

#include <reactor/exception.hh>
#include <reactor/network/buffer.hh>
#include <reactor/network/resolve.hh>
#include <reactor/network/udt-server.hh>
#include <reactor/operation.hh>
#include <reactor/scheduler.hh>

#include <elle/printf.hh>
#include <elle/idiom/Close.hh>
#include <elle/log.hh>
#include <elle/nat/Nat.hh>

#include <common/common.hh>

#include <cryptography/random.hh>

ELLE_LOG_COMPONENT("reactor.network.UDTServer");

namespace reactor
{
  namespace network
  {

    /*-------------.
    | Construction |
    `-------------*/

    UDTServer::UDTServer(Scheduler& sched)
      : Super(sched)
      , _accepted()
      , _sockets()
      , _udp_socket(nullptr)
    {}

    UDTServer::~UDTServer()
    {
      if (_heartbeat)
        _heartbeat->terminate();
    }

    /*----------.
    | Accepting |
    `----------*/

    class UDTAccept: public Operation
    {
      public:
        UDTAccept(Scheduler& scheduler, boost::asio::ip::udt::acceptor& acceptor)
          : Operation(scheduler)
          , _acceptor(acceptor)
          , _socket(0)
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
            _raise(new Exception(scheduler(), error.message()));
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

    static std::string escape(std::string const& str)
    {
      std::string res("\"");
      std::for_each(str.begin(), str.end(),
                     [&res](char c)
                     {
                       if (c == '\n')
                         res += "\\n";
                       else
                         res += c;
                     });
      return res + "\"";
    }

    bool
    UDTServer::_punch(int port)
    {
      boost::asio::ip::udp::endpoint local_endpoint
        (boost::asio::ip::udp::v4(), port);
      std::unique_ptr<UDPSocket> socket(new UDPSocket(scheduler()));
      socket->bind(local_endpoint);
      return this->_punch(port, socket);
    }

    bool
    UDTServer::_punch(int port, std::unique_ptr<UDPSocket>& socket)
    {
      ELLE_DEBUG_SCOPE("try punching port %s", port);
      static auto lhost = "development.infinit.io";//common::longinus::host();
      static auto lport = 9999;//common::longinus::port();
      static auto longinus =
        resolve_udp(this->scheduler(), lhost,
                    boost::lexical_cast<std::string>(lport));

      std::stringstream ss;
      ss << "local " << socket->local_endpoint() << std::endl;
      std::string question = ss.str();
      ELLE_DUMP("longinus question: %s", escape(question));
      socket->send_to(Buffer(question), longinus);

      std::string buffer_data(1024, ' ');
      Buffer buffer(buffer_data);
      auto size = socket->read_some(buffer);
      std::string answer(buffer_data.c_str(), size);
      ELLE_DUMP("longinus answer: %s", escape(answer));

      std::vector<std::string> splitted;
      boost::split(splitted, answer, boost::is_any_of(" :\n"));
      if (splitted.size() != 2)
        throw reactor::Exception(elle::concurrency::scheduler(),
                                 "loginus returned a bad formed endpoint: " +
                                 answer);
      boost::asio::ip::udp::endpoint public_endpoint
        (boost::asio::ip::address::from_string(splitted[1]),
         boost::lexical_cast<int>(splitted[2]));
      if (public_endpoint.port() == port)
        {
          ELLE_DEBUG("punched right port %s", port);
          if (!_udp_socket)
            {
              _public_endpoint = public_endpoint;
              _udp_socket = std::move(socket);
              _heartbeat.reset
                (new reactor::Thread
                 (this->scheduler(),
                  elle::sprintf("%s punch heartbeat", *this),
                  [this] ()
                  {
                    while (true)
                      {
                        this->scheduler().current()->sleep
                          (boost::posix_time::seconds(15));
                        this->_punch(this->port(),
                                     std::ref(this->_udp_socket));
                      }
                  }));
            }
          return true;
        }
      else
        {
          ELLE_DEBUG("punched different port %s", public_endpoint.port());
          if (_udp_socket)
            ;  // FIXME: we lost the NAT, do something.
          return false;
        }
    }

    void
    UDTServer::listen(int desired_port)
    {
      // Randomize port manually
      if (desired_port == 0)
        while (desired_port <= 1024)
          desired_port =
            infinit::cryptography::random::generate<elle::Natural16>();

      // Punch the potential firewall
      ELLE_TRACE("punch hole in the firewall")
      {
        int port = desired_port;
        try
          {
            int tries = 0;
            while (true)
              {
                if (tries++ == 10)
                  {
                    ELLE_WARN("too many tries, giving up");
                    break;
                  }
                if (this->_punch(port))
                  break;
                ++port;
              }

          }
        catch (std::runtime_error const& e)
          {
            ELLE_WARN("NAT punching error: %s", e.what());
          }
      }
      if (this->_udp_socket == nullptr)
        {
          this->_udp_socket =
            std::unique_ptr<UDPSocket>(new UDPSocket(this->scheduler()));
          this->_udp_socket->bind(boost::asio::ip::udp::endpoint
                                  (boost::asio::ip::udp::v4(), desired_port));
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
