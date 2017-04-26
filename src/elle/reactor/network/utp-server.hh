#pragma once

#include <elle/reactor/Barrier.hh>
#include <elle/reactor/network/fwd.hh>
#include <elle/reactor/network/rdv-socket.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /// Server accepting UTP connections and providing UTPSockets.
      class UTPServer
      {
      /*-------------.
      | Construction |
      `-------------*/
      public:
        /// Construct an UTP server.
        ///
        /// XXX: Add xorify to the constructor?
        UTPServer();

      /*-----------.
      | Attributes |
      `-----------*/
      public:
        /// Apply a xor on every single bytes with the given char.
        ELLE_attribute_rw(unsigned char, xorify);

      /*-----------.
      | Networking |
      `-----------*/
      public:
        using EndPoint = boost::asio::ip::udp::endpoint;
        /// @see Server::listen.
        void
        listen(int port, bool ipv6 = false);
        /// @see Server::listen.
        void
        listen(boost::asio::ip::address host,
               int port = 0, bool enable_ipv6 = false);
        /// @see Server::listen.
        void
        listen(EndPoint const& end_point);
        /// @see Server::local_endpoint.
        EndPoint
        local_endpoint();
        /// @see Server::accept.
        std::unique_ptr<UTPSocket>
        accept();
        /// Operate a rdv_connection on the given address.
        ///
        /// XXX: Default port is 7890.
        ///
        /// \param id The ID used to identify us.
        /// \param address The RDV server address (xx.xx.xx.xx[:port]).
        /// \param timeout The maximum duration the RDV can take.
        void
        rdv_connect(std::string const& id,
                    std::string const& address,
                    DurationOpt timeout = DurationOpt());
        /// @see RDVSocket::set_local_id.
        void
        set_local_id(std::string const& id);
        /// @see RDVSocket::rdv_connected.
        bool
        rdv_connected() const;
        /// Take ownership of the RDV socket.
        std::unique_ptr<RDVSocket> const&
        socket();
      private:
        friend class UTPSocket;

      /*---------------.
      | Implementation |
      `---------------*/
      public:
        struct Impl;
      private:
        ELLE_ATTRIBUTE(std::shared_ptr<Impl>, impl);
      };
    }
  }
}
