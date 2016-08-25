#ifndef REACTOR_NETWORK_UTP_SOCKET_HH
# define REACTOR_NETWORK_UTP_SOCKET_HH

# include <boost/asio.hpp>

# include <elle/Buffer.hh>
# include <elle/Duration.hh>
# include <elle/IOStream.hh>

#include <reactor/network/fwd.hh>

// FIXME: hide those
typedef struct UTPSocket utp_socket;
typedef struct struct_utp_context utp_context;

namespace reactor
{
  namespace network
  {
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
      UTPSocket(UTPServer& server, utp_socket* socket, bool open);
      UTPSocket(UTPServer& server, std::string const& host, int port);
      ~UTPSocket();

    public:
      void
      connect(std::string const& host, int port);
      void
      connect(std::string const& id,
              std::vector<EndPoint> const& endpoints = {},
              elle::DurationOpt timeout = {});
      void
      write(elle::ConstWeakBuffer const& data, elle::DurationOpt timeout = {});
      elle::Buffer
      read(size_t sz, elle::DurationOpt timeout = {});
      elle::Buffer
      read_some(size_t sz, elle::DurationOpt timeout = {});
      elle::Buffer
      read_until(std::string const& delimiter, elle::DurationOpt opt = {});
      void
      close();
      void
      stats();
      EndPoint
      peer() const;

    /*----------.
    | Printable |
    `----------*/
    public:
      virtual
      void
      print(std::ostream& output) const override;

    /*---------------.
    | Implementation |
    `---------------*/
    public:
      struct Impl;
    private:
      ELLE_ATTRIBUTE(std::unique_ptr<Impl>, impl);
    };
  }
}

#endif
