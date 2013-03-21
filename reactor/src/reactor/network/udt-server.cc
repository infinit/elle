#include <algorithm>
#include <random>
#include <chrono>

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
#include <elle/nat/Nat.hh>
#include <elle/os/getenv.hh>

ELLE_LOG_COMPONENT("reactor.network.UDTServer");

namespace /*annon*/
{
  class PunchException:
    public elle::Exception
  {
  public:
    PunchException(elle::String const& message):
      Exception(elle::sprintf("punch failed: %s", message))
    {}
    PunchException(elle::Exception const& e):
      PunchException(e.what())
    {}
  };

  class HeartbeatFailed:
    public elle::Exception
  {
  public:
    HeartbeatFailed(elle::String const& message):
      Exception(elle::sprintf("heartbeat failed: %s", message))
    {}
    HeartbeatFailed(elle::Exception const &e):
      HeartbeatFailed(e.what())
    {}
  };

  class PunchTimeout:
    public PunchException
  {
  public:
    PunchTimeout(elle::String const& message):
      PunchException(elle::sprintf("timed out: %s", message))
    {}
    PunchTimeout(elle::Exception const &e):
      PunchTimeout(e.what())
    {}
  };

  class PunchFormat:
    public PunchException
  {
  public:
    PunchFormat(elle::String const& message):
      PunchException(elle::sprintf("format error: %s", message))
    {}
    PunchFormat(elle::Exception const &e):
      PunchFormat(e.what())
    {}
  };
} /*annon*/

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
      _udp_socket(nullptr)
    {}

    UDTServer::~UDTServer()
    {
      if (_heartbeat)
        _heartbeat->terminate_now();
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
            _raise(new Exception(error.message()));
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
      auto edit = [&] (char c)
      {
        if (isspace(c))
        {
          res += '\\';
          switch (c)
          {
            case '\f': res += 'f'; break;
            case '\n': res += 'n'; break;
            case '\r': res += 'r'; break;
            case '\t': res += 't'; break;
            case '\v': res += 'v'; break;
          }
        }
        else if (!isgraph(c))
        {
          res += elle::sprintf("\\x%02x", static_cast<int>(c));
        }
        else
          res += c;
      };
      std::for_each(str.begin(), str.end(), edit);
      return res + "\"";
    }

    namespace longinus
    {
      std::string const&
      host()
      {
        static std::string const host_string = elle::os::getenv(
          "INFINIT_LONGINUS_HOST",
          elle::sprint("production.infinit.io")
          );

        return host_string;
      }

      int
      port()
      {
        static std::string const port_string = elle::os::getenv(
          "INFINIT_LONGINUS_PORT",
          "9999"
          );
        return std::stoi(port_string);
      }
    }

    boost::asio::ip::udp::endpoint const&
    UDTServer::_longinus()
    {
      static auto lhost = longinus::host();
      static auto lport = longinus::port();
      static auto longinus =
        resolve_udp(this->scheduler(), lhost,
                    boost::lexical_cast<std::string>(lport));

      return longinus;
    }

    boost::asio::ip::udp::endpoint
    UDTServer::_punch(int port, std::unique_ptr<UDPSocket>& socket)
    {
      static auto longinus = _longinus();
      return _punch(port, socket, longinus);
    }

    boost::asio::ip::udp::endpoint
    UDTServer::_punch(int port,
                      std::unique_ptr<UDPSocket>& socket,
                      boost::asio::ip::udp::endpoint const& longinus)
    {
      ELLE_DEBUG_SCOPE("try punching port %s", port);
      ELLE_DEBUG("contact longinus on %s", longinus);

      std::stringstream ss;
      ss << "local " << socket->local_endpoint() << std::endl;
      std::string question = ss.str();
      socket->send_to(Buffer(question), longinus);
      ELLE_DUMP("longinus question: %s", escape(question));

      std::string buffer_data(1024, ' ');
      Buffer buffer(buffer_data);
      //XXX: make timeout parametrizable
      int size;
      try
      {
        size = socket->read_some(buffer, boost::posix_time::seconds(15));
      }
      catch (reactor::network::TimeOut const& e)
      {
        // transform this into a PunchTimeout
        throw PunchTimeout("read_some");
      }
      std::string answer(buffer_data.c_str(), size);
      ELLE_DUMP("longinus answer: %s", escape(answer));

      //XXX
      std::vector<std::string> splitted;
      boost::split(splitted, answer, boost::is_any_of(" :\n"));
      if (splitted.size() != 4)
      {
        // The heartbeat failed, this is a serious bug, but we can't fix it now.
        throw PunchFormat("loginus endpoint: " + escape(answer));
      }

      ELLE_ASSERT(splitted[1] != "0.0.0.0");

      boost::asio::ip::udp::endpoint public_endpoint
        (boost::asio::ip::address::from_string(splitted[1]),
         boost::lexical_cast<int>(splitted[2]));
      return public_endpoint;
    }

    bool
    UDTServer::_punch_heartbeat()
    {
      ELLE_TRACE("Heartbeating the NAT punching");
      try
      {
          auto port = this->_public_endpoint.port();
          auto endpoint = this->_punch(port, this->_udp_socket);
          if (endpoint.port() != port)
          {
              ELLE_WARN("NAT punching was lost");
              // XXX: we lost the NAT, do something.
              return false;
          }
          else
          {
              ELLE_DEBUG("NAT punching still up");
              return true;
          }
      }
      catch (PunchException const &e)
      {
        // Throw a Heartbeat Failed here
        throw HeartbeatFailed(e);
      }
      return false;
    }

    void
    UDTServer::listen(int desired_port)
    {
      // Randomize port manually
      if (desired_port == 0)
        {
          auto seed =
            std::chrono::system_clock::now().time_since_epoch().count();

          std::default_random_engine generator(seed);

          desired_port = (generator() % (65535 - 1025)) + 1025;

          ELLE_ASSERT((desired_port > 1024) && (desired_port <= 65535));
        }

      // Punch the potential firewall
      ELLE_TRACE("punch hole in the firewall")
      {
        int port = desired_port;
        try
          {
            int tries = 0;
            std::unique_ptr<UDPSocket> socket;
            boost::asio::ip::udp::endpoint public_endpoint;
            while (true)
              {
                boost::asio::ip::udp::endpoint local_endpoint
                  (boost::asio::ip::udp::v4(), port);
                socket.reset(new UDPSocket(scheduler()));
                socket->bind(local_endpoint);
                public_endpoint = this->_punch(port, socket);
                if (public_endpoint.port() == port)
                  {
                    ELLE_DEBUG("punched right port %s", port);
                    break;
                  }
                else
                  ELLE_DEBUG("punched different port %s",
                             public_endpoint.port());
                ++port;
                if (tries++ == 10)
                  {
                    ELLE_WARN("too many tries, giving up");
                    break;
                  }
              }
            this->_udp_socket = std::move(socket);
            this->_public_endpoint = public_endpoint;
            this->_heartbeat.reset
              (new reactor::Thread
               (this->scheduler(),
                elle::sprintf("%s punch heartbeat", *this),
                [this] ()
                {
                  while (true)
                    {
                      this->scheduler().current()->sleep
                        (boost::posix_time::seconds(15));
                      try {
                        this->_punch_heartbeat();
                      } catch (HeartbeatFailed const &e)
                      {
                        ELLE_WARN("XXX %s", e.what());
                      }
                    }
                }));
            ELLE_TRACE("checking NAT punching with second longinus")
              {
                static auto longinus_2 = resolve_udp(this->scheduler(),
                                                     "development.infinit.io",
                                                     "9999");
                auto res = this->_punch(this->_public_endpoint.port(),
                                        this->_udp_socket,
                                        longinus_2);
                if (res.port() == this->_public_endpoint.port())
                  ELLE_LOG("NAT punching worked with port: %s", res.port());
                else
                  ELLE_WARN("NAT punching failed with port: %s", res.port());
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
