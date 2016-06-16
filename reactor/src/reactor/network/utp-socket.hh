#ifndef REACTOR_NETWORK_UTP_SOCKET_HH
# define REACTOR_NETWORK_UTP_SOCKET_HH

# include <deque>

# include <boost/asio.hpp>

# include <reactor/Barrier.hh>
# include <reactor/MultiLockBarrier.hh>
# include <reactor/network/rdv-socket.hh>

typedef struct UTPSocket utp_socket;
typedef struct struct_utp_context utp_context;

namespace reactor
{
  namespace network
  {
    class UTPServer;

    class UTPSocket
      : public elle::IOStream
    {
    public:
      typedef boost::asio::ip::udp::endpoint EndPoint;
      UTPSocket(UTPServer& server);
      UTPSocket(UTPServer& server, utp_socket* socket, bool open);
      UTPSocket(UTPServer& server, std::string const& host, int port);
      ~UTPSocket();
      void
      connect(std::string const& host, int port);
      void
      connect(std::string const& id,
              std::vector<EndPoint> const& endpoints = {},
              DurationOpt timeout = DurationOpt());
      void
      write(elle::ConstWeakBuffer const& data, DurationOpt timeout = DurationOpt());
      elle::Buffer
      read(size_t sz, DurationOpt timeout = DurationOpt());
      elle::Buffer
      read_some(size_t sz, DurationOpt timeout = DurationOpt());
      elle::Buffer
      read_until(std::string const& delimiter, DurationOpt opt = DurationOpt());
      void
      close();
      void
      stats();
      // For internal use
      void
      write_cont();
      void
      on_connect();
      void
      on_close();
      void
      on_read(elle::ConstWeakBuffer const&);
      void
      destroyed();
      EndPoint
      peer();
    private:
      void
      _read();
      ELLE_ATTRIBUTE(elle::Buffer, read_buffer);
      ELLE_ATTRIBUTE(Barrier, read_barrier);
      ELLE_ATTRIBUTE(Barrier, write_barrier);
      ELLE_ATTRIBUTE(Mutex, write_mutex);
      ELLE_ATTRIBUTE(Barrier, connect_barrier);
      ELLE_ATTRIBUTE_R(UTPServer&, server);
      ELLE_ATTRIBUTE(utp_socket*, socket);
      ELLE_ATTRIBUTE(elle::ConstWeakBuffer, write);
      ELLE_ATTRIBUTE(MultiLockBarrier, pending_operations);
      ELLE_ATTRIBUTE(int, write_pos);
      ELLE_ATTRIBUTE(bool, open);
      ELLE_ATTRIBUTE(bool, closing);
    };
  }
}

#endif
