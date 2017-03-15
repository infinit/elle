#pragma once

#include <elle/reactor/asio.hh>
#include <elle/reactor/network/socket.hh>
#include <elle/reactor/signal.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /// A PlainSocket designed for UDP connections.
      class UDPSocket
        : public PlainSocket<boost::asio::ip::udp::socket>
      {
      /*------.
      | Types |
      `------*/
      public:
        using Super = PlainSocket<boost::asio::ip::udp::socket>;
        using AsioResolver = boost::asio::ip::udp::resolver;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        /// Create an unbound UDPSocket using given Scheduler.
        ///
        /// \param scheduler The Scheduler to use.
        UDPSocket(Scheduler& scheduler);
        /// Create an unbound UDPSocket using the current Scheduler.
        UDPSocket();
        /// Create and bind an UDPSocket.
        ///
        /// \param hostname The name of the host.
        /// \param port The port the host is listening to.
        UDPSocket(std::string const& hostname,
                  std::string const& port);
        /// Create and bind an UDPSocket.
        ///
        /// \param scheduler The Scheduler to use.
        /// \param hostname The name of the host.
        /// \param port The port the host is listening to.
        UDPSocket(Scheduler& sched,
                  std::string const& hostname,
                  std::string const& port);
        /// Create an bind an UDPSocket.
        ///
        /// @see UDPSocket(std::string, std::string).
        UDPSocket(std::string const& hostname,
                  int port);
        /// Create and bind an UDPSocket.
        ///
        /// @see UDPSocket(Scheduler, std::string, std::string).
        UDPSocket(Scheduler& sched,
                  std::string const& hostname,
                  int port);
        virtual
        ~UDPSocket();

      /*--------------.
      | Configuration |
      `--------------*/
      public:
        /// Bind the UDPSocket to the given Endpoint.
        ///
        /// \param endpoint The endpoint to connect to.
        void
        bind(EndPoint const& endpoint);

      /*-----.
      | Read |
      `-----*/
      public:
        /// @see Socket::read_some.
        ///
        /// In UDPSocket, this means read data from a connected Socket.
        Size
        read_some(elle::WeakBuffer buffer,
                  DurationOpt timeout = {},
                  int* bytes_read = nullptr) override;
        /// @see Socket::read_until.
        ///
        /// In UDPSocket, this means read data from a connected Socket.
        elle::Buffer
        read_until(std::string const& delimiter,
                   DurationOpt opt = DurationOpt()) override;
        /// Read data from an EndPoint.
        ///
        /// \param buffer The destination buffer.
        /// \param endpoint The EndPoint to read data from.
        Size
        receive_from(elle::WeakBuffer buffer,
                     boost::asio::ip::udp::endpoint& endpoint,
                     DurationOpt timeout = {});


      /*------.
      | Write |
      `------*/
      public:
        /// @see Socket::write.
        ///
        /// In UDPSocket, this means send data to a connected Socket.
        void
        write(elle::ConstWeakBuffer buffer) override;
        /// Send data to an EndPoint.
        ///
        /// \param buffer Payload to send.
        /// \param endpoint The endpoint to send data to.
        void
        send_to(elle::ConstWeakBuffer buffer,
                EndPoint endpoint);

      /*----------------.
      | Pretty printing |
      `----------------*/
      public:
        void
        print(std::ostream& s) const override;
      };
    }
  }
}
