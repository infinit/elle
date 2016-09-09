#ifndef REACTOR_NETWORK_UTP_SERVER_HH
# define REACTOR_NETWORK_UTP_SERVER_HH

# include <reactor/Barrier.hh>
# include <reactor/network/fwd.hh>
# include <reactor/network/rdv-socket.hh>

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
      typedef boost::asio::ip::udp::endpoint EndPoint;
      void
      listen(int port, bool ipv6 = false);
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

#endif
