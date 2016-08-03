#ifndef REACTOR_NETWORK_UTP_SERVER_HH
# define REACTOR_NETWORK_UTP_SERVER_HH

# include <deque>

# include <reactor/Barrier.hh>
# include <reactor/network/fwd.hh>
# include <reactor/network/rdv-socket.hh>
# include <reactor/network/utp-socket.hh>

namespace reactor
{
  namespace network
  {
    class UTPServer
    {
    public:
      UTPServer();
      ~UTPServer();
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
      // For internal use
      void
      send_to(Buffer buf, EndPoint where);
      void
      on_accept(utp_socket* s);
      bool
      rdv_connected() const;
    private:
      void
      _check_icmp();
      void
      _cleanup();
      std::function<void(boost::system::error_code const&, size_t)> send_cont;
      utp_context* ctx;
      ELLE_ATTRIBUTE_R(std::unique_ptr<RDVSocket>, socket);
      ELLE_ATTRIBUTE_RX(unsigned char, xorify);
      std::vector<std::unique_ptr<UTPSocket>> _accept_queue;
      Barrier _accept_barrier;
      std::unique_ptr<Thread> _listener;
      std::unique_ptr<Thread> _checker;
      std::deque<std::pair<elle::Buffer, EndPoint>> _send_buffer;
      bool _sending;
      int _icmp_fd;
      friend class UTPSocket;
    };
  }
}

#endif
