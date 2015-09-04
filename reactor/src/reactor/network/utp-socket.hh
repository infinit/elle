#ifndef REACTOR_NETWORK_UTP_SOCKET_HH
#define REACTOR_NETWORK_UTP_SOCKET_HH

#include <deque>

#include <boost/asio.hpp>
#include <reactor/network/udp-socket.hh>
#include <reactor/Barrier.hh>

typedef struct UTPSocket					utp_socket;
typedef struct struct_utp_context			utp_context;
namespace reactor
{
  namespace network
  {
    class UTPSocket;
    class UTPServer
    {
    public:
      UTPServer();
      ~UTPServer();
      typedef boost::asio::ip::udp::endpoint EndPoint;
      void
      listen(int port);
      void
      listen(EndPoint const& end_point);
      EndPoint
      local_endpoint();
      std::unique_ptr<UTPSocket>
      accept();

      // For internal use
      void
      send_to(Buffer buf, EndPoint where);
      void
      on_accept(utp_socket* s);
    private:
      void
      _check_icmp();
      std::function<void(boost::system::error_code const&, size_t)> send_cont;
      utp_context* ctx;
      std::unique_ptr<UDPSocket> _socket;
      std::vector<std::unique_ptr<UTPSocket>> _accept_queue;
      Barrier _accept_barrier;
      std::unique_ptr<Thread> _listener;
      std::unique_ptr<Thread> _checker;
      std::deque<std::pair<elle::Buffer, EndPoint>> _send_buffer;
      bool _sending;
      friend class UTPSocket;
    };

    class UTPSocket: public elle::IOStream
    {
    public:
      UTPSocket(UTPServer& server);
      UTPSocket(UTPServer& server, utp_socket* socket);
      UTPSocket(UTPServer& server, std::string const& host, int port);
      ~UTPSocket();
      void
      connect(std::string const& host, int port);
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
    private:
      void
      _read();
      elle::Buffer _read_buffer;
      Barrier _read_barrier;
      Barrier _write_barrier;
      Mutex _write_mutex;
      Barrier _connect_barrier;
      UTPServer&  _server;
      utp_socket* _socket;
      elle::ConstWeakBuffer _write;
      int _write_pos;
      bool _open;
    };
  }
}

#endif