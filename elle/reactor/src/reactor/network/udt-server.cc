#include <algorithm>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

#include <common/common.hh>

#include <elle/printf.hh>
#include <elle/idiom/Close.hh>
#include <elle/log.hh>
#include <elle/nat/Nat.hh>

#include <asio-udt/acceptor.hh>
#include <reactor/exception.hh>
#include <reactor/network/buffer.hh>
#include <reactor/network/resolve.hh>
#include <reactor/network/udt-server.hh>
#include <reactor/operation.hh>
#include <reactor/scheduler.hh>


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
    {}

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
      this->_sockets.push_back(std::unique_ptr<UDTSocket>
                               (new UDTSocket(this->scheduler(), addr, port)));
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

    void
    UDTServer::listen(int desired_port)
    {
      // Punch the potential firewall
      {
        int port = desired_port;
        try
          {
            auto lhost = common::longinus::host();
            auto lport = common::longinus::port();
            auto longinus =
              resolve_udp(this->scheduler(), lhost,
                          boost::lexical_cast<std::string>(lport));
            ELLE_TRACE_SCOPE("punch hole with %s:%d", lhost, lport);
            int tries = 0;
            while (true)
              {
                if (tries++ == 10)
                  {
                    ELLE_WARN("too many tries, giving up");
                    break;
                  }
                ELLE_DEBUG_SCOPE("try punching port %s", port);
                boost::asio::ip::udp::endpoint local_endpoint
                  (boost::asio::ip::udp::v4(), port);
                std::unique_ptr<UDPSocket> socket(new UDPSocket(scheduler()));
                socket->bind(local_endpoint);
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
                boost::asio::ip::udp::endpoint public_endpoint
                  (boost::asio::ip::address::from_string(splitted[1]),
                   boost::lexical_cast<int>(splitted[2]));
                if (public_endpoint.port() == port)
                  {
                    _public_endpoint = public_endpoint;
                    _udp_socket = std::move(socket);
                    ELLE_DEBUG("punched right port %s", port);
                    break;
                  }
                ELLE_DEBUG("punched different port %s", public_endpoint.port());
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
      // try
      //   {
      //     if (_udp_socket)
      //       {
      //         auto fd = _udp_socket->socket()->native_handle();
      //         _acceptor = new boost::asio::ip::udt::acceptor
      //           (scheduler().io_service(), port, fd);
      //       }
      //     else
      //       _acceptor = new boost::asio::ip::udt::acceptor
      //         (scheduler().io_service(), port);
      //   }
      // catch (boost::system::system_error& e)
      //   {
      //     throw Exception(scheduler(),
      //                     elle::sprintf("unable to listen on %s: %s",
      //                                   port, e.what()));
      //   }
    }

    // void
    // UDTServer::listen_fd(int port, int fd)
    // {
    //   try
    //     {
    //       _acceptor = new boost::asio::ip::udt::acceptor
    //         (scheduler().io_service(), port, fd);
    //     }
    //   catch (boost::system::system_error& e)
    //     {
    //       throw Exception(scheduler(),
    //                       elle::sprintf("unable to listen on %s: %s",
    //                                     port, e.what()));
    //     }
    // }

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

  }
}
