#pragma once

#include <unordered_map>

#include <elle/reactor/network/server.hh>
#include <elle/reactor/network/udp-socket.hh>
#include <elle/reactor/network/udp-server-socket.hh>
#include <elle/reactor/signal.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      struct HashEndpoint
      {
        std::size_t operator()(const boost::asio::ip::udp::endpoint&) const;
        std::size_t operator()(const boost::asio::ip::tcp::endpoint&) const;
      };

      /// A specialized ProtoServer to provide UDPSockets.
      ///
      /// XXX[doc].
      class UDPServer
        : public Server, public ProtoServer<UDPSocket>
      {
      public:
        using Super = Server;
        /// Create an UDPServer from a given Scheduler.
        UDPServer(Scheduler& sched);
        ~UDPServer();

      /*----------.
      | Listening |
      `----------*/
      public:
        /// Listen on the given port.
        ///
        /// \param port The port to listen to.
        /// \param enable_ipv6 Whether it accepts IPv6 connections.
        void
        listen(int port, bool enable_ipv6=false);
        /// Listen on the given EndPoint.
        ///
        /// \param endpoint The EndPoint.
        void
        listen(const EndPoint& endpoint);

      /*----------.
      | Accepting |
      `----------*/
      public:
        /// Get a UDPSocket bound to a peer.
        UDPServerSocket*
        accept();
      private:
        friend class UDPServerSocket;
        /// XXX: Why a raw pointer?
        boost::asio::ip::udp::socket* _acceptor;

      /*---------------.
      | Demultiplexing |
      `---------------*/
      private:
        void
        _receive();
        void
        _receive_handle(const boost::system::error_code& error,
                        std::size_t bytes_transferred);
        char _buffer[512];
        EndPoint _peer;
        using Clients = std::unordered_map<EndPoint, UDPServerSocket*,
                                           HashEndpoint>;
        Clients _clients;
        Signal _accept;
        /// XXX: Use Channel instead.
        using Sockets = std::vector<UDPServerSocket*>;
        Sockets _accepted;

      /*---------.
      | Printing |
      `---------*/
      public:
        void
        print(std::ostream& s) const;
      };

      std::ostream& operator << (std::ostream& s, const UDPServer& server);
    }
  }
}
