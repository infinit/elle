#pragma once

#include <reactor/network/udp-socket.hh>
#include <reactor/Barrier.hh>
#include <reactor/MultiLockBarrier.hh>
#include <reactor/duration.hh>
#include <reactor/thread.hh>

namespace reactor
{
  namespace network
  {
    /** UDP socket with added nat traversal facility, using an accessible
    *   RDV server.
    *   The underlying UDP socket must be bound and unconnected()
    */
    class RDVSocket: public UDPSocket
    {
    public:
      using Endpoint = boost::asio::ip::udp::endpoint;
      RDVSocket();
      ~RDVSocket();
      // connect to rdv server
      void rdv_connect(std::string const& id,
                       std::string const& rdv_host, int rdv_port,
                       DurationOpt timeout = DurationOpt());
      void rdv_connect(std::string const& id,
                       Endpoint server,
                       DurationOpt timeout = DurationOpt());
      void set_local_id(std::string const& id);
      /// We expect user to call receive_from in a loop, before rdv_connect.
      Size
      receive_from(Buffer buffer,
                   boost::asio::ip::udp::endpoint &endpoint,
                   DurationOpt timeout = DurationOpt());
      /// Contact an RDV-aware peer.
      Endpoint
      contact(std::string const& id,
              std::vector<Endpoint> const& endpoints = {},
              DurationOpt timeout = DurationOpt());
      bool rdv_connected() const;

      using reader = std::function<void(Buffer, Endpoint)>;
      // Register a function that will be called for each received packet
      // begining with magic. Magic must be 8 bytes long.
      void
      register_reader(std::string const& magic, reader data_callback);
      void unregister_reader(std::string const& magic);
      ELLE_ATTRIBUTE_R(Endpoint, public_endpoint);

    private:
      void send_to_failsafe(Buffer buffer, Endpoint endpoint);
      void send_ping(Endpoint target, std::string const& tid = {});
      void loop_breach();
      void loop_keep_alive();
      void send_with_magik(elle::Buffer const& buf, Endpoint target);
      std::string _id;
      Endpoint _server;
      Endpoint _self;
      Barrier _server_reached;
      struct ContactInfo
      {
        ContactInfo(RDVSocket const& owner, std::string const& id);
        void set_result(Endpoint endpoint);
        Barrier barrier;
        boost::optional<Endpoint> result;
        boost::posix_time::ptime result_time;
        int waiters;
      };
      std::unordered_map<std::string, ContactInfo> _contacts;
      std::vector<std::pair<Endpoint, int>> _breach_requests;
      std::unordered_map<std::string, reader> _readers;
      reactor::Thread _breacher;
      reactor::Thread _keep_alive;
      MultiLockBarrier _tasks;
    };
  }
}
