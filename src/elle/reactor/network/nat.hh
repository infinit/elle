#pragma once

#include <algorithm>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <elle/attribute.hh>
#include <elle/network/Locus.hh>
#include <elle/reactor/network/udp-socket.hh>
#include <elle/reactor/scheduler.hh>

namespace elle
{
  namespace reactor
  {
    namespace nat
    {

      inline boost::posix_time::seconds
      operator "" _sec(const char* str)
      {
        return boost::posix_time::seconds{std::stoi(str)};
      }

      class Hole
      {
      private:
        std::unique_ptr<reactor::network::UDPSocket>      _handle;
        boost::asio::ip::udp::endpoint                    _longinus;

        ELLE_ATTRIBUTE_R(boost::asio::ip::udp::endpoint, public_endpoint);
      public:
        std::unique_ptr<reactor::network::UDPSocket>
        handle();

      public:
        Hole() = delete;
        Hole(Hole const& hole) = delete;
        Hole& operator =(Hole const& other) = delete;
        Hole& operator =(Hole&& other) = delete;
        Hole(Hole&& hole);
        ~Hole() = default;

        Hole(reactor::Scheduler& sched,
             boost::asio::ip::udp::endpoint const& longinus,
             int local_port = 0);
      private:
        boost::asio::ip::udp::endpoint
        _punch(unsigned short port, boost::posix_time::seconds timeout = 10_sec);
      };

#if defined(REACTOR_HAVE_STUN)
      class Breach
      {
      private:
        std::unique_ptr<reactor::network::UDPSocket> _handle;
        boost::asio::ip::udp::endpoint _stunserver;

      public:
        enum class NatBehavior
        {
          /// Unknown Behavior, is there a bug somewhere ?
          UnknownBehavior,
          /// IP address and port are the same between client and server view
          /// (NO NAT)
          DirectMapping,
          /// same mapping regardless of IP:port original packet sent to (the
          /// kind of NAT we like)
          EndpointIndependentMapping,
          /// mapping changes for local socket based on remote IP address only,
          /// but remote port can change (partially symmetric, not great)
          AddressDependentMapping,
          /// different port mapping if the ip address or port change (symmetric
          /// NAT, difficult to predict port mappings)
          AddressAndPortDependentMapping
        };

      private:
        ELLE_ATTRIBUTE_R(boost::asio::ip::udp::endpoint, mapped_endpoint);
        ELLE_ATTRIBUTE_R(boost::asio::ip::udp::endpoint, local_endpoint);
        ELLE_ATTRIBUTE_R(NatBehavior, nat_behavior);

      public:
        std::unique_ptr<reactor::network::UDPSocket>
        take_handle();

      public:
        Breach() = delete;
        Breach(Breach const&) = delete;
        Breach& operator =(Breach const&) = delete;
        Breach& operator =(Breach&&) = delete;
        ~Breach() = default;

        Breach(Breach&& breach);

        Breach(reactor::Scheduler& sched,
               boost::asio::ip::udp::endpoint const& stunserver,
               unsigned short local_port = 0);
      };
#endif

      class NAT
      {
      public:

      public:
        NAT(reactor::Scheduler& s);
        ~NAT() = default;

      public:
        Hole
        punch(std::string const& hostname,
              int port,
              int local_port = 0);

#if defined(REACTOR_HAVE_STUN)
        Breach
        map(boost::asio::ip::udp::endpoint const& endpoint,
            unsigned short local_port = 0);
#endif

      private:
        reactor::Scheduler&         sched;
      };

    } /* nat */
  } /* reactor */
} /* elle */
