#ifndef REACTOR_NETWORK_UTP_SOCKET_HH
# define REACTOR_NETWORK_UTP_SOCKET_HH

# include <boost/asio.hpp>

# include <elle/Buffer.hh>
# include <elle/IOStream.hh>

# include <reactor/Barrier.hh>
# include <reactor/MultiLockBarrier.hh>
# include <reactor/mutex.hh>
# include <reactor/network/fwd.hh>

// FIXME: hide those
typedef struct UTPSocket utp_socket;
typedef struct struct_utp_context utp_context;

namespace reactor
{
  namespace network
  {
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
      elle::Buffer _read_buffer;
      Barrier _read_barrier;
      Barrier _write_barrier;
      Mutex _write_mutex;
      Barrier _connect_barrier;
      Barrier _destroyed_barrier;
      ELLE_ATTRIBUTE_R(UTPServer&, server);
      utp_socket* _socket;
      elle::ConstWeakBuffer _write;
      MultiLockBarrier _pending_operations;
      int _write_pos;
      bool _open;
      bool _closing;
    };
  }
}

#endif
