#pragma once

#include <elle/reactor/network/udp-socket.hh>
#include <elle/reactor/Barrier.hh>
#include <elle/reactor/MultiLockBarrier.hh>
#include <elle/reactor/duration.hh>
#include <elle/reactor/Thread.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /// UDP socket with added nat traversal facility, using an accessible
      /// RDV server.
      ///
      /// The underlying UDP socket must be bound and unconnected().
      class RDVSocket
        : public UDPSocket
      {
      public:
        using Endpoint = boost::asio::ip::udp::endpoint;
        RDVSocket();
        ~RDVSocket();
        /// Connect to RDV server.
        ///
        /// \param id The ID use to identify us.
        /// \param rdv_host The host name of the RDV server.
        /// \param rdv_port The port used by the RDV server.
        /// \param timeout The maximum duration the RDV can take.
        void
        rdv_connect(std::string const& id,
                    std::string const& rdv_host,
                    int rdv_port,
                    DurationOpt timeout = DurationOpt());
        /// Connect to RDV server.
        ///
        /// \param id The ID used to identify us.
        /// \param server The EndPoint of the RDV server.
        /// \param timeout The maximum duration the RDV can take.
        void
        rdv_connect(std::string const& id,
                    Endpoint server,
                    DurationOpt timeout = DurationOpt());
        /// Set local ID.
        ///
        /// \param id Our new ID.
        void
        set_local_id(std::string const& id);
        /// Receive data from the RDV server.
        ///
        /// We expect user to call receive_from in a loop, before rdv_connect.
        ///
        /// \param buffer The WeakBuffer to be filled.
        /// \param endpoint Will contain the Endpoint of the peer.
        /// \param timeout The maximum duration before it times out.
        Size
        receive_from(elle::WeakBuffer buffer,
                     boost::asio::ip::udp::endpoint& endpoint,
                     DurationOpt timeout = DurationOpt());
        /// Contact an RDV-aware peer.
        ///
        /// \param id ID if the peer.
        /// \param endpoints The extra EndPoints to contact.
        /// \param timeout The maximum duration before it times out.
        /// \returns Endpoint of the peer.
        Endpoint
        contact(std::string const& id,
                std::vector<Endpoint> const& endpoints = {},
                DurationOpt timeout = DurationOpt());
        /// Whether the RDV connection is established.
        bool
        rdv_connected() const;

        using Reader = std::function<void(elle::WeakBuffer, Endpoint)>;
        /// Register a function that will be called for each received packet
        /// begining with magic. Magic must be 8 bytes long.
        ///
        /// \param magic An 8-bytes long magic.
        /// \param data_callback The callback to be called.
        void
        register_reader(std::string const& magic, Reader data_callback);
        /// Unregister reader associated to a magic.
        ///
        /// \param magic An 8-bytes long magic.
        void
        unregister_reader(std::string const& magic);
        ELLE_ATTRIBUTE_R(Endpoint, public_endpoint);

      private:
        void
        _send_to_failsafe(elle::ConstWeakBuffer buffer, Endpoint endpoint);
        void
        _send_ping(Endpoint target, std::string const& tid = {});
        void
        _loop_breach();
        void
        _loop_keep_alive();
        void
        _send_with_magik(elle::Buffer const& buf, Endpoint target);
        ELLE_ATTRIBUTE(std::string, id);
        ELLE_ATTRIBUTE(Endpoint, server);
        ELLE_ATTRIBUTE(Endpoint, self);
        ELLE_ATTRIBUTE(Barrier, server_reached);
        struct ContactInfo
        {
          ContactInfo(RDVSocket const& owner, std::string const& id);
          void
          set_result(Endpoint endpoint);
          Barrier barrier;
          boost::optional<Endpoint> result;
          boost::posix_time::ptime result_time;
          int waiters;
        };
        using Contacts = std::unordered_map<std::string, ContactInfo>;
        ELLE_ATTRIBUTE(Contacts, contacts);
        using BreachRequests = std::vector<std::pair<Endpoint, int>>;
        ELLE_ATTRIBUTE(BreachRequests, breach_requests);
        using Readers = std::unordered_map<std::string, Reader>;
        ELLE_ATTRIBUTE(Readers, readers);
        ELLE_ATTRIBUTE(reactor::Thread, breacher);
        ELLE_ATTRIBUTE(reactor::Thread, keep_alive);
        ELLE_ATTRIBUTE(MultiLockBarrier, tasks);
      };
    }
  }
}
