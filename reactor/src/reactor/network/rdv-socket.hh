#ifndef REACTOR_NETWORK_RDV_SOCKET_HH
# define REACTOR_NETWORK_RDV_SOCKET_HH

#include <reactor/network/udp-socket.hh>
#include <reactor/Barrier.hh>
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
      // We expect user to call receive_from in a loop, before rdv_connect
      Size receive_from(Buffer buffer,
        boost::asio::ip::udp::endpoint &endpoint,
        DurationOpt timeout = DurationOpt());
      // Contact a RDV-aware peer
      Endpoint contact(std::string const& id,
        std::vector<Endpoint> const& endpoints = {},
        DurationOpt timeout = DurationOpt());
      bool rdv_connected() const;
      private:
        void send_ping(Endpoint target);
        void loop_breach();
        void loop_keep_alive();
        void send_with_magik(elle::Buffer const& buf, Endpoint target);
        std::string _id;
        Endpoint _server;
        Endpoint _self;
        Barrier _server_reached;
        struct ContactInfo
        {
          std::shared_ptr<Barrier> barrier;
          boost::optional<Endpoint> result;
        };
        std::unordered_map<std::string, ContactInfo> _contacts;
        std::vector<std::pair<Endpoint, int>> _breach_requests;
        reactor::Thread _breacher;
        reactor::Thread _keep_alive;
    };
  }
}

#endif