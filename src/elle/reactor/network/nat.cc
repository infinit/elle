#include <random>
#include <string>
#include <sstream>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/asio/ip/udp.hpp>

#include <elle/memory.hh>
#include <elle/reactor/network/exception.hh>
#include <elle/reactor/network/nat.hh>
#include <elle/reactor/network/resolve.hh>
#include <elle/reactor/sleep.hh>
#include <elle/reactor/Thread.hh>

#if defined(REACTOR_HAVE_STUN)
# include <stun/common/commonincludes.hpp>
# include <stun/common/oshelper.h>
# include <stun/stuncore/stuncore.h>
# include <stun/stuncore/socketaddress.h>
#endif

ELLE_LOG_COMPONENT("elle.reactor.NAT");

namespace asio = boost::asio;
namespace ip = boost::asio::ip;
namespace network = reactor::network;

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
    HeartbeatFailed(elle::Exception const& e):
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
    PunchTimeout(elle::Exception const& e):
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
    PunchFormat(elle::Exception const& e):
      PunchFormat(e.what())
    {}
  };
} /*annon*/

namespace elle
{
  namespace reactor
  {
    namespace nat
    {

      Hole::Hole(reactor::Scheduler& sched,
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
            this->_handle.reset(new network::UDPSocket{sched});

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

      std::unique_ptr<network::UDPSocket>
      Hole::handle()
      {
        return std::move(this->_handle);
      }

      boost::asio::ip::udp::endpoint
      Hole::_punch(unsigned short port, boost::posix_time::seconds timeout)
      {
        ELLE_DEBUG_SCOPE("try punching port %s", port);

        std::string question{"echo sasp"};
        this->_handle->send_to(elle::ConstWeakBuffer(question), _longinus);

        auto buffer_data = std::string(1024, ' ');
        auto buffer = elle::WeakBuffer(buffer_data);
        int size;
        try
        {
          size = this->_handle->read_some(buffer, timeout);
        }
        catch (network::TimeOut const& e)
        {
          throw PunchTimeout("read_some");
        }
        auto answer = std::string(buffer_data.c_str(), size);
        ELLE_DUMP("longinus said: %s", answer);

        // XXX This is not optimal
        auto splitted = std::vector<std::string>{};
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

#if defined REACTOR_HAVE_STUN
      static std::string NatBehaviorToString(NatBehavior behavior)
      {
        std::string pretty_behavior;

        switch (behavior)
        {
          default:
          case UnknownBehavior:
            pretty_behavior = "Unknown Behavior"; break;
          case DirectMapping:
            pretty_behavior = "Direct Mapping"; break;
          case EndpointIndependentMapping:
            pretty_behavior = "Endpoint Independent Mapping"; break;
          case AddressDependentMapping:
            pretty_behavior = "Address Dependent Mapping"; break;
          case AddressAndPortDependentMapping:
            pretty_behavior = "Address and Port Dependent Mapping"; break;
        }
        return pretty_behavior;
      }

      static std::string NatFilteringToString(NatFiltering filtering)
      {
        std::string behavior;

        switch (filtering)
        {
          default:
          case UnknownFiltering:
            behavior = "Unknown Behavior"; break;
          case DirectConnectionFiltering:
            behavior = "Direct Mapping"; break;
          case EndpointIndependentFiltering:
            behavior = "Endpoint Independent Filtering"; break;
          case AddressDependentFiltering:
            behavior = "Address Dependent Filtering"; break;
          case AddressAndPortDependentFiltering:
            behavior = "Address and Port Dependent Filtering"; break;
        }
        return behavior;
      }

      static
      void
      DumpResults(StunClientLogicConfig& config,
                  StunClientResults& results)
      {
        char buff[100];
        std::string result;

        if (results.fBindingTestSuccess)
        {
          results.addrLocal.ToStringBuffer(buff);
          ELLE_DEBUG("Local address: %s", buff);

          results.addrMapped.ToStringBuffer(buff);
          ELLE_DEBUG("Mapped address: %s", buff);
        }

        if (config.fBehaviorTest)
        {

          ELLE_DEBUG("Behavior test: %s",
                     results.fBehaviorTestSuccess ? "success" : "fail");
          if (results.fBehaviorTestSuccess)
          {
            result = NatBehaviorToString(results.behavior);
            ELLE_DEBUG("Nat behavior: %s", result.c_str());
          }
        }

        if (config.fFilteringTest)
        {
          ELLE_DEBUG("Filtering test: %s",
                     results.fBehaviorTestSuccess ? "success" : "fail");
          if (results.fBehaviorTestSuccess)
          {
            result = NatFilteringToString(results.filtering);
            ELLE_DEBUG("Nat filtering: %s", result.c_str());
          }
        }
      }

      static
      ip::udp::endpoint
      to_endpoint(std::string const& str)
      {
        std::vector<std::string> v;

        boost::split(v, str, boost::is_any_of(":"));
        return {ip::address::from_string(v[0]), (unsigned short)std::stoi(v[1])};
      }

      Hole::Hole(Hole&& rhs):
        _handle{nullptr},
        _longinus{std::move(rhs._longinus)},
        _public_endpoint{std::move(rhs._public_endpoint)}
      {
        std::swap(this->_handle, rhs._handle);
      }

      Breach::Breach(reactor::Scheduler& sched,
                     boost::asio::ip::udp::endpoint const& stunserver,
                     unsigned short local_port):
        _handle{new network::UDPSocket{sched}},
        _stunserver{stunserver}
      {
        // Bind the socket to the local_port.
        boost::asio::ip::udp::endpoint end{
          boost::asio::ip::udp::v4(),
          local_port
        };

        this->_handle->bind(end);

        // Stun config: {
        //   addr,
        //   timeout is instant,
        //   timeout,
        //   max attempts,
        //   Do NAT behavior test,
        //   Do NAT filtering test,
        // }
        StunClientLogicConfig config{
          CSocketAddress(*_stunserver.data()),
            false, 0, 10, true, false
        };

        CStunClientLogic clientlogic;
        clientlogic.Initialize(config);

        CRefCountedBuffer spMsg(new CBuffer(MAX_STUN_MESSAGE_SIZE));
        CSocketAddress addrDest; // To whom we send.

        while (true)
        {
          HRESULT r = clientlogic.GetNextMessage(spMsg, &addrDest,
                                                 GetMillisecondCounter());
          if (SUCCEEDED(r))
          {
            sockaddr const* s = addrDest.GetSockAddr();
            sockaddr_in const* sin = (struct sockaddr_in*)s;

            boost::asio::ip::address_v4 addr(ntohl(sin->sin_addr.s_addr));
            boost::asio::ip::udp::endpoint dest{addr, ntohs(sin->sin_port)};

            auto b = elle::WeakBuffer(spMsg->GetData(), spMsg->GetSize());
            this->_handle->send_to(b, dest);
          }
          else if (r == E_STUNCLIENT_RESULTS_READY)
          {
            break;
          }
          else if (r == E_STUNCLIENT_STILL_WAITING)
          {
            ELLE_DUMP("continuing to wait for response...");
            continue;
          }
          else
            ELLE_ERR("Fatal error !");

          boost::asio::ip::udp::endpoint remote_stun;
          auto b = elle::WeakBuffer(spMsg->GetData(), spMsg->GetAllocatedSize());
          try
          {
            int size = this->_handle->receive_from(b, remote_stun, 500_ms);
            CSocketAddress addrRemote(*remote_stun.data());
            CSocketAddress addrLocal(*this->_handle->local_endpoint().data());
            spMsg->SetSize(size);
            clientlogic.ProcessResponse(spMsg, addrRemote, addrLocal);
          }
          catch (network::TimeOut const&e)
          {
            continue;
          }
        }
        StunClientResults results;
        results.Init();
        clientlogic.GetResults(&results);
        DumpResults(config, results);
        if (results.fBindingTestSuccess)
        {
          char buff[100];
          results.addrMapped.ToStringBuffer(buff);
          this->_mapped_endpoint = to_endpoint(buff);

          results.addrLocal.ToStringBuffer(buff);
          this->_local_endpoint = to_endpoint(buff);
        }
        if (results.fBehaviorTestSuccess)
        {
          switch (results.behavior)
          {
            case UnknownBehavior:
              this->_nat_behavior = NatBehavior::UnknownBehavior;
              break;
            case DirectMapping:
              this->_nat_behavior = NatBehavior::DirectMapping;
              break;
            case EndpointIndependentMapping:
              this->_nat_behavior = NatBehavior::EndpointIndependentMapping;
              break;
            case AddressDependentMapping:
              this->_nat_behavior = NatBehavior::AddressDependentMapping;
              break;
            case AddressAndPortDependentMapping:
              this->_nat_behavior = NatBehavior::AddressAndPortDependentMapping;
              break;
          }
        }
      }

      Breach::Breach(Breach&& rhs):
        _handle{std::move(rhs._handle)},
        _stunserver{std::move(rhs._stunserver)},
        _mapped_endpoint{std::move(rhs._mapped_endpoint)}
      {
      }

      std::unique_ptr<network::UDPSocket>
      Breach::take_handle()
      {
        return std::move(this->_handle);
      }

      Breach
      NAT::map(boost::asio::ip::udp::endpoint const& hostname,
               unsigned short local_port)
      {
        return Breach(sched, hostname, local_port);
      }
#endif

      NAT::NAT(reactor::Scheduler& s):
        sched(s)
      {}

      Hole
      NAT::punch(std::string const& hostname,
                 int port,
                 int local_port)
      {
        auto longinus =
          network::resolve_udp(sched, hostname, std::to_string(port));
        return Hole(sched, longinus, local_port);
      }
    } /* nat */
  } /* reactor */
} /* elle */
