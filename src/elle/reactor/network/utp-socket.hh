#pragma once

#include <boost/asio.hpp>

#include <elle/Buffer.hh>
#include <elle/Duration.hh>
#include <elle/IOStream.hh>
#include <elle/reactor/network/fwd.hh>
#include <elle/reactor/network/utp-server.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /// An Socket implementing uTP (Micro Transport Protocol).
      class UTPSocket
        : public elle::IOStream
        , public elle::Printable
      {
      /*-------------.
      | Construction |
      `-------------*/
      public:
        using EndPoint = boost::asio::ip::udp::endpoint;
        UTPSocket(UTPServer& server);
        UTPSocket(UTPServer& server, std::string const& host, int port);
        ~UTPSocket();

      public:
        /// Connect to the given host / port.
        ///
        /// \param host The host name.
        /// \param port The port the host is listening to.
        void
        connect(std::string const& host, int port);
        /// Connect to an host by asking EndPoints for its address (knowing its
        /// ID).
        ///
        /// @see RDVSocket::contact.
        ///
        /// \param id The ID of the peer to connect to.
        /// \param endpoints The extra EndPoints to contact.
        /// \param timeout The maximum duration before it times out.
        /// \returns The Endpoint of the peer.
        void
        connect(std::string const& id,
                std::vector<EndPoint> const& endpoints = {},
                elle::DurationOpt timeout = {});
        /// @see Socket::write.
        void
        write(elle::ConstWeakBuffer const& data, elle::DurationOpt timeout = {});
        /// @see Socket::read.
        elle::Buffer
        read(size_t sz, elle::DurationOpt timeout = {});
        /// @see Socket::read_some.
        elle::Buffer
        read_some(size_t sz, elle::DurationOpt timeout = {});
        /// @see Socket::read_until.
        elle::Buffer
        read_until(std::string const& delimiter, elle::DurationOpt opt = {});
        /// Close the connection.
        void
        close();
        /// Print stats of the underlying socket.
        void
        stats();
        /// Get the EndPoint of the peer.
        EndPoint
        peer() const;

      /*----------.
      | Printable |
      `----------*/
      public:
        void
        print(std::ostream& output) const override;

      /*---------------.
      | Implementation |
      `---------------*/
      public:
        struct Impl;
      private:
        UTPSocket(std::unique_ptr<Impl> impl);
        friend class UTPServer;
        ELLE_ATTRIBUTE(std::unique_ptr<Impl>, impl);
      };
    }
  }
}
