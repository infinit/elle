#ifndef REACTOR_NETWORK_UTP_SERVER_HH
# define REACTOR_NETWORK_UTP_SERVER_HH

# include <deque>

# include <boost/asio.hpp>

# include <reactor/Barrier.hh>
# include <reactor/network/rdv-socket.hh>
# include <reactor/network/utp-socket.hh>

typedef struct UTPSocket utp_socket;
typedef struct struct_utp_context utp_context;

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

    private:
      typedef
        std::function<void(boost::system::error_code const&, size_t)>
        SendContent;
      ELLE_ATTRIBUTE(SendContent, send_cont);
      // ELLE_ATTRIBUTE_RX(std::shared_ptr<utp_context>, ctx);
      ELLE_ATTRIBUTE_RX(utp_context*, ctx);
      ELLE_ATTRIBUTE_R(std::unique_ptr<RDVSocket>, socket);
      ELLE_ATTRIBUTE_RX(unsigned char, xorify);
      ELLE_ATTRIBUTE(std::vector<std::unique_ptr<UTPSocket>>, accept_queue);
      ELLE_ATTRIBUTE(Barrier, accept_barrier);
      ELLE_ATTRIBUTE(std::unique_ptr<Thread>, listener);
      ELLE_ATTRIBUTE(std::unique_ptr<Thread>, checker);
      typedef std::deque<std::pair<elle::Buffer, EndPoint>> SendBuffer;
      ELLE_ATTRIBUTE(SendBuffer, send_buffer);
      ELLE_ATTRIBUTE(bool, sending);
      friend class UTPSocket;
    };
  }
}

#endif
