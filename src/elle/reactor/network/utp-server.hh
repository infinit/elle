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
      class UTPServer
      {
      /*-------------.
      | Construction |
      `-------------*/
      public:
        UTPServer();
        ~UTPServer();

      /*-----------.
      | Attributes |
      `-----------*/
      public:
        ELLE_attribute_rw(unsigned char, xorify);

      /*-----------.
      | Networking |
      `-----------*/
      public:
        using EndPoint = boost::asio::ip::udp::endpoint;
        void
        listen(int port, bool ipv6 = false);
        void
        listen(boost::asio::ip::address host,
               int port = 0, bool enable_ipv6 = false);
        void
        listen(EndPoint const& end_point);
        EndPoint
        local_endpoint();
        std::unique_ptr<UTPSocket>
        accept();
        void
        rdv_connect(std::string const& id,
                    std::string const& address,
                    DurationOpt timeout = DurationOpt());
        void
        set_local_id(std::string const& id);
        bool
        rdv_connected() const;
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
