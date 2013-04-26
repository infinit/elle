#include <string>
#include <sstream>

#include <elle/container/vector.hh>
#include <elle/memory.hh>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/asio/ip/udp.hpp>

#include <reactor/network/nat.hh>

#include <reactor/network/buffer.hh>
#include <reactor/network/resolve.hh>
#include <reactor/sleep.hh>
#include <reactor/thread.hh>
#include <reactor/network/exception.hh>

ELLE_LOG_COMPONENT("reactor.NAT");

namespace asio = boost::asio;
namespace ip = boost::asio::ip;

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
  namespace nat
  {

    Hole::Hole(reactor::Scheduler &sched,
               boost::asio::ip::udp::endpoint const& longinus,
               int local_port):
      _handle{nullptr},
      _longinus{longinus}
    {
      // Randomize port manually
      if (local_port == 0)
      {
        auto seed =
          std::chrono::system_clock::now().time_since_epoch().count();

        std::default_random_engine generator(seed);

        local_port = (generator() % (65535 - 1025)) + 1025;

        ELLE_ASSERT((local_port > 1024) && (local_port <= 65535));
      }
      ELLE_TRACE("nat punching: %s", local_port)
      {
        ELLE_DEBUG("contact longinus on %s", this->_longinus);
        static const int max_tries = 10;

        unsigned short port = local_port;
        int tries = 0;
        boost::asio::ip::udp::endpoint public_endpoint;
        for (;tries < max_tries; ++tries, ++port)
        {
          ip::udp::endpoint local_endpoint{boost::asio::ip::udp::v4(), port};
          this->_handle.reset(new reactor::network::UDPSocket{sched});

          this->_handle->bind(local_endpoint);
          public_endpoint = this->_punch(port);

          if (public_endpoint.port() == port)
          {
            ELLE_DEBUG("punched right port %s", port);
            break;
          }
          else
            ELLE_DEBUG("punched different port %s", public_endpoint.port());
        }
        if (tries == max_tries)
          throw PunchException{
            elle::sprintf("maximum tries(%d) exceeded", max_tries)
          };
        this->_public_endpoint = public_endpoint;
      }
    }

    std::unique_ptr<reactor::network::UDPSocket>
    Hole::handle()
    {
      return std::move(this->_handle);
    }

    boost::asio::ip::udp::endpoint
    Hole::_punch(unsigned short port, boost::posix_time::seconds timeout)
    {
      ELLE_DEBUG_SCOPE("try punching port %s", port);

      std::string question{"echo sasp"};
      this->_handle->send_to(reactor::network::Buffer(question), _longinus);

      std::string buffer_data(1024, ' ');
      reactor::network::Buffer buffer(buffer_data);
      int size;
      try
      {
        size = this->_handle->read_some(buffer, timeout);
      }
      catch (reactor::network::TimeOut const& e)
      {
        throw PunchTimeout("read_some");
      }
      std::string answer(buffer_data.c_str(), size);
      ELLE_DUMP("longinus said: %s", answer);

      // XXX This is not optimal
      std::vector<std::string> splitted;
      boost::split(splitted, answer, boost::is_any_of(" :\n"));
      if (splitted.size() != 3)
      {
        // The heartbeat failed, this is a serious bug, but we can't fix it now.
        throw PunchFormat{
          elle::sprintf("loginus endpoint: <%d bytes>", answer.size())
        };
      }
      ELLE_DEBUG("longinus answer: %s", splitted);

      ELLE_ASSERT(splitted[1] != "0.0.0.0");

      boost::asio::ip::udp::endpoint public_endpoint;
      std::string const& addr = splitted[0];
      unsigned short remote_port = std::stoi(splitted[1]);
      return ip::udp::endpoint{ip::address::from_string(addr), remote_port};
    }


    Hole::Hole(Hole&& rhs):
      _handle{nullptr},
      _longinus{std::move(rhs._longinus)},
      _public_endpoint{std::move(rhs._public_endpoint)}
    {
      std::swap(this->_handle, rhs._handle);
    }

    NAT::NAT(reactor::Scheduler &s):
      sched(s)
    {

    }

    Hole
    NAT::punch(std::string const &hostname,
               int port,
               int local_port)
    {
      auto longinus =
        reactor::network::resolve_udp(sched, hostname, std::to_string(port));
      return Hole(sched, longinus, local_port);
    }

  } /* nat */
} /* elle */
