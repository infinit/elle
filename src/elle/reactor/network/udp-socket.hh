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
      class UDPSocket
        : public PlainSocket<boost::asio::ip::udp::socket>
      {
        /*---------.
        | Typedefs |
        `---------*/
        public:
          using Super = PlainSocket<boost::asio::ip::udp::socket>;
          using AsioResolver = boost::asio::ip::udp::resolver;

        /*-------------.
        | Construction |
        `-------------*/
        public:
          UDPSocket();
          UDPSocket(Scheduler& sched);
          UDPSocket(const std::string& hostname,
                    const std::string& port);
          UDPSocket(Scheduler& sched,
                    const std::string& hostname,
                    const std::string& port);
          UDPSocket(const std::string& hostname,
                    int port);
          UDPSocket(Scheduler& sched,
                    const std::string& hostname,
                    int port);
          virtual
          ~UDPSocket();

        /*--------------.
        | Configuration |
        `--------------*/
        public:
          void
          bind(boost::asio::ip::udp::endpoint const& endpoint);

        /*-----.
        | Read |
        `-----*/
        public:
          Size
          read_some(elle::WeakBuffer buffer,
                    DurationOpt timeout = DurationOpt(),
                    int* bytes_read = nullptr) override;

          Size
          receive_from(elle::WeakBuffer buffer,
                       boost::asio::ip::udp::endpoint &endpoint,
                       DurationOpt timeout = DurationOpt());

        /*------.
        | Write |
        `------*/
        public:
          void
          write(elle::ConstWeakBuffer buffer) override;
          elle::Buffer
          read_until(std::string const& delimiter,
                     DurationOpt opt = DurationOpt()) override;
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
