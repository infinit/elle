#pragma once

#include <algorithm>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <elle/attribute.hh>
#include <elle/network/Locus.hh>
#include <elle/reactor/network/udp-socket.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/Duration.hh>

namespace elle
{
  namespace reactor
  {
    namespace nat
    {
      /// DISCLAIMER.
      ///
      /// This is deprecated in favor of RDVSocket.
      ///
      /// However, everything should work just fine.

      /// Local connection to an Hole, for hole punching.
      ///
      /// A hole is a third party that can relay external and internal address
      /// and port for each client.
      class Hole
      {
      private:
        ELLE_ATTRIBUTE(std::unique_ptr<reactor::network::UDPSocket>, handle);
        ELLE_ATTRIBUTE(boost::asio::ip::udp::endpoint, hole_endpoint);

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

        /// Construct a Hole.
        ///
        /// \param scheduler The Scheduler to use.
        /// \param hole_endpoint The UDP EndPoint of the Hole.
        /// \param local_port The local port to force.
        Hole(reactor::Scheduler& scheduler,
             boost::asio::ip::udp::endpoint const& hole_endpoint,
             int local_port = 0);
      private:
        boost::asio::ip::udp::endpoint
        _punch(unsigned short port, boost::posix_time::seconds timeout = 10_sec);
      };

#if defined(REACTOR_HAVE_STUN)
      /// Breach for NAT traversal.
      ///
      /// Use STUN to figure out local and mapped endpoints.
      class Breach
      {
      private:
        ELLE_ATTRIBUTE(std::unique_ptr<reactor::network::UDPSocket>, handle);
        ELLE_ATTRIBUTE(boost::asio::ip::udp::endpoint, stun_endpoint);

      public:
        enum class NatBehavior
        {
          /// Unknown Behavior, is there a bug somewhere ?
          UnknownBehavior,
          /// IP address and port are the same between client and server view
          /// (NO NAT)
          DirectMapping,
          /// Same mapping regardless of IP:port original packet sent to (the
          /// kind of NAT we like)
          EndpointIndependentMapping,
          /// Mapping changes for local socket based on remote IP address only,
          /// but remote port can change (partially symmetric, not great)
          AddressDependentMapping,
          /// Different port mapping if the ip address or port change (symmetric
          /// NAT, difficult to predict port mappings)
          AddressAndPortDependentMapping
        };

      private:
        ELLE_ATTRIBUTE_R(boost::asio::ip::udp::endpoint, mapped_endpoint);
        ELLE_ATTRIBUTE_R(boost::asio::ip::udp::endpoint, local_endpoint);
        ELLE_ATTRIBUTE_R(NatBehavior, nat_behavior);

      public:
        /// Take ownership of the UDPSocket used.
        std::unique_ptr<reactor::network::UDPSocket>
        take_handle();

      public:
        Breach() = delete;
        Breach(Breach const&) = delete;
        Breach& operator =(Breach const&) = delete;
        Breach& operator =(Breach&&) = delete;
        ~Breach() = default;

        Breach(Breach&& breach);
        /// Construct a Breach.
        ///
        /// \param scheduler The Scheduler to use.
        /// \param hole_endpoint The UDP EndPoint of the STUN server.
        /// \param local_port The local port to force.
        Breach(reactor::Scheduler& sched,
               boost::asio::ip::udp::endpoint const& stunserver,
               unsigned short local_port = 0);
      };
#endif

      /// Helper around Hole and Breach.
      class NAT
      {
      public:
        NAT(reactor::Scheduler& s);
        ~NAT() = default;

      public:
        /// Create a Hole.
        ///
        /// \param hostname The host name of the hole.
        /// \param port The port used by the hole.
        /// \param local_port The local port to force.
        Hole
        punch(std::string const& hostname,
              int port,
              int local_port = 0);

#if defined(REACTOR_HAVE_STUN)
        /// Create a Breach.
        ///
        /// \param endpoint The UDP EndPoint of the STUN server.
        /// \param local_port The local port to force.
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
