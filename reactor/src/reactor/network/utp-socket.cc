#include <reactor/network/utp-socket.hh>
#include <reactor/network/buffer.hh>
#include <reactor/thread.hh>
#include <reactor/scheduler.hh>
#include <reactor/exception.hh>
#include <reactor/network/exception.hh>
#include <elle/log.hh>
#include <utp.h>

ELLE_LOG_COMPONENT("reactor.network.utp");

namespace reactor
{
  
  namespace network
  {
static uint64 on_sendto(utp_callback_arguments* args)
{
  ELLE_DEBUG("on_sendto %s", args->len);
  struct sockaddr_in *sin = (struct sockaddr_in *) args->address;
  UTPServer::EndPoint ep(
    boost::asio::ip::address::from_string(inet_ntoa(sin->sin_addr)),
    ntohs(sin->sin_port));
  UTPServer* server = (UTPServer*)utp_context_get_userdata(args->context);
  server->send_to(Buffer(args->buf, args->len), ep);
  return 0;
}

static uint64 on_read(utp_callback_arguments* args)
{
  ELLE_DEBUG("on_read");
  UTPSocket* sock = (UTPSocket*)utp_get_userdata(args->socket);
  if (!sock)
    return 0;
  sock->_read_buffer.append(args->buf, args->len);
  utp_read_drained(args->socket);
  sock->_read();
  return 0;
}

static uint64 on_firewall(utp_callback_arguments *a)
{
  return 0;
}

static uint64 on_accept(utp_callback_arguments* args)
{
  ELLE_DEBUG("on_accept");
  UTPServer* server = (UTPServer*)utp_context_get_userdata(args->context);
  server->_accept_queue.emplace_back(new UTPSocket(*server, args->socket));
  server->_accept_barrier.open();
  return 0;
}

static uint64 on_error(utp_callback_arguments* args)
{
  ELLE_DEBUG("on_error %s", utp_error_code_names[args->error_code]);
  UTPSocket* s = (UTPSocket*) utp_get_userdata(args->socket);
  if (!s)
    return 0;
  s->_close();
  return 0;
}

static uint64 on_state_change(utp_callback_arguments* args)
{
  ELLE_DEBUG("on_state_change %s", utp_state_names[args->state]);
  UTPSocket* s = (UTPSocket*) utp_get_userdata(args->socket);
  if (!s)
    return 0;
  switch(args->state)
  {
    case UTP_STATE_CONNECT:
		case UTP_STATE_WRITABLE:
		  if (s->_write.size())
		  {
		    unsigned char* data = const_cast<unsigned char*>(s->_write.contents());
		    int sz = s->_write.size();
		    while (s->_write_pos < sz)
		    {
		      ssize_t len = utp_write(s->_socket, data + s->_write_pos, sz - s->_write_pos);
		      if (!len)
		      {
		        ELLE_DEBUG("from status: write buffer full");
		        break;
		      }
		      s->_write_pos += len;
		    }
		    if (s->_write_pos == sz)
		      s->_write_barrier.open();
		  }
		  //s->_write_barrier.open();
		  break;
		case UTP_STATE_EOF:
		  s->_close();
		  break;
		case UTP_STATE_DESTROYING:
		  s->_close();
		  break;
  }
  return 0;
}

static uint64 on_connect(utp_callback_arguments* args)
{
  ELLE_DEBUG("on_connect");
  UTPSocket* s = (UTPSocket*) utp_get_userdata(args->socket);
  s->_connect_barrier.open();
  s->_write_barrier.open();
  return 0;
}

static uint64 on_log(utp_callback_arguments* args)
{
  ELLE_DEBUG("utp: %s", args->buf);
  return 0;
}

std::unique_ptr<UTPSocket> UTPServer::accept()
{
  _accept_barrier.wait();
  ELLE_ASSERT(_accept_barrier.opened());
  std::unique_ptr<UTPSocket> sock(_accept_queue.back().release());
  _accept_queue.pop_back();
  if (_accept_queue.empty())
    _accept_barrier.close();
  return std::move(sock);
}

UTPServer::UTPServer()
{
  _sending = false;
  ctx = utp_init(2);
  utp_context_set_userdata(ctx, this);
  utp_set_callback(ctx, UTP_ON_FIREWALL, &on_firewall);
  utp_set_callback(ctx, UTP_ON_ACCEPT, &on_accept);
  utp_set_callback(ctx, UTP_ON_ERROR, &on_error);
  utp_set_callback(ctx, UTP_ON_STATE_CHANGE, &on_state_change);
  utp_set_callback(ctx, UTP_ON_READ, &on_read);
  utp_set_callback(ctx, UTP_ON_CONNECT, &on_connect);
  utp_set_callback(ctx, UTP_SENDTO, &on_sendto);
  utp_set_callback(ctx, UTP_LOG, &on_log);
}

UTPServer::~UTPServer()
{
  ELLE_DEBUG("Terminating");
  if (!_socket)
    return; // was never initialized
  _socket->close();
  _socket->socket()->close();
  if (_checker)
  {
    _checker->terminate();
    reactor::wait(*_checker);
    ELLE_DEBUG("checker down");
  }
  ELLE_DEBUG("listener teardown");
  if (_listener)
  {
    _listener->terminate();
    reactor::wait(*_listener);
    ELLE_DEBUG("listener down");
  }

  utp_destroy(ctx);
}

void UTPServer::send_to(Buffer buf, EndPoint where)
{
  _send_buffer.emplace_back(elle::Buffer(buf.data(), buf.size()), where);
  if (!_sending)
  {
    _sending = true;
    send_cont =
    static_cast<decltype(send_cont)>([this](boost::system::error_code const& erc, size_t sz)
    {
      if (erc)
        ELLE_WARN("send_to error: %s", erc.message());
      _send_buffer.pop_front();
      if (_send_buffer.empty())
      {
        _sending = false;
      }
      else
      {
        _socket->socket()->async_send_to(boost::asio::buffer(
            _send_buffer.front().first.contents(),
            _send_buffer.front().first.size()),
          _send_buffer.front().second,
          this->send_cont);
      }
    });
    _socket->socket()->async_send_to(boost::asio::buffer(
        _send_buffer.front().first.contents(),
        _send_buffer.front().first.size()),
      _send_buffer.front().second,
      send_cont);
  }
}

auto UTPServer::local_endpoint() -> EndPoint
{
  auto ep = _socket->local_endpoint();
  return EndPoint(ep.address(), ep.port());
}

UTPSocket::UTPSocket(UTPServer& server)
: UTPSocket(server, utp_create_socket(server.ctx))
{
}

UTPSocket::~UTPSocket()
{
  ELLE_DEBUG("~UTPSocket");
  _close();
  ELLE_DEBUG("~UTPSocket finished");
}

void UTPSocket::_close()
{
  if (!_socket)
    return;
  utp_close(_socket);
  _open = false;
  _read_barrier.open();
  _write_barrier.open();
  utp_set_userdata(_socket, nullptr);
  _socket = nullptr;
}

namespace
{
  class StreamBuffer: public elle::DynamicStreamBuffer
  {
  public:
    StreamBuffer(UTPSocket* socket)
    : DynamicStreamBuffer(65536)
    , _socket(socket)
    {}
    Size read(char* buffer, Size size) override
    {
      elle::Buffer buf = _socket->read_some(size);
      memcpy(buffer, buf.contents(), buf.size());
      return buf.size();
    }
    void write(char* buffer, Size size) override
    {
      _socket->write(elle::ConstWeakBuffer(buffer, size));
    }
    UTPSocket* _socket;
  };
}

UTPSocket::UTPSocket(UTPServer& server, utp_socket* socket)
: IOStream(new StreamBuffer(this))
, _server(server)
, _socket(socket)
, _open(true)
{
  utp_set_userdata(_socket, this);
  _write_barrier.open();
  ELLE_DEBUG("snd %s recv %s", utp_getsockopt(_socket, UTP_SNDBUF),
    utp_getsockopt(_socket, UTP_RCVBUF));
}

void UTPSocket::close()
{
  _close();
}

void UTPSocket::connect(std::string const& host, int port)
{
  struct addrinfo * ai;
  addrinfo hints;
  memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
  getaddrinfo(host.c_str(), std::to_string(port).c_str(),
              &hints, &ai);
  utp_connect(_socket, ai->ai_addr, ai->ai_addrlen);
  freeaddrinfo(ai);
  _connect_barrier.wait();
}

void UTPSocket::write(elle::ConstWeakBuffer const& buf, DurationOpt opt)
{
  using namespace boost::posix_time;
  if (!_open)
    throw SocketClosed();
  ptime start = microsec_clock::universal_time();
  Lock l(_write_mutex);
  unsigned char* data = const_cast<unsigned char*>(buf.contents());
  int sz = buf.size();
  _write = buf;
  _write_pos = 0;
  while (_write_pos < sz)
  {
    ssize_t len = utp_write(_socket, data + _write_pos, sz - _write_pos);
    if (!len)
    {
      ELLE_DEBUG("write buffer full");
      _write_barrier.close();
      Duration elapsed = microsec_clock::universal_time() - start;
      if (opt && *opt < elapsed)
        throw TimeOut();
      _write_barrier.wait(opt? elapsed - *opt: opt);
      ELLE_DEBUG("write woken up");
      if (!_open)
        throw SocketClosed();
      continue;
    }
    _write_pos += len;
  }
  _write_pos = 0;
  _write = {};
}

void UTPSocket::_read()
{
  _read_barrier.open();
}

void UTPSocket::stats()
{
  utp_socket_stats* st = utp_get_stats(_socket);
  if (st == nullptr)
    return;
  std::cerr << "recv " << st->nbytes_recv << "\nsent " << st->nbytes_xmit
  << "\nrexmit " << st->rexmit << "\nfastrexmit " << st->fastrexmit
  << "\nnxmit " << st->nxmit <<"\nnrecv" << st->nrecv
  << "\nnduprect " << st->nduprecv <<"\nmtu " << st->mtu_guess << std::endl;
}

elle::Buffer UTPSocket::read_until(std::string const& delimiter, DurationOpt opt)
{
  using namespace boost::posix_time;
  if (!_open)
    throw SocketClosed();
  ptime start = microsec_clock::universal_time();
  while (true)
  {
    size_t p = _read_buffer.string().find(delimiter);
    if (p != std::string::npos)
    {
      return read(p + delimiter.length());
    }
    _read_barrier.close();
    Duration elapsed = microsec_clock::universal_time() - start;
    if (opt && *opt < elapsed)
      throw TimeOut();
    _read_barrier.wait(opt? elapsed - *opt: opt);
    if (!_open)
      throw SocketClosed();
  }
}

elle::Buffer UTPSocket::read(size_t sz, DurationOpt opt)
{
  using namespace boost::posix_time;
  if (!_open)
    throw SocketClosed();
  ELLE_DEBUG("read");
  ptime start = microsec_clock::universal_time();
  while (_read_buffer.size() < sz)
  {
    ELLE_DEBUG("read wait %s", _read_buffer.size());
    _read_barrier.close();
    Duration elapsed = microsec_clock::universal_time() - start;
    if (opt && *opt < elapsed)
      throw TimeOut();
    _read_barrier.wait(opt? elapsed - *opt: opt);
    ELLE_DEBUG("read wake %s", _read_buffer.size());
    if (!_open)
      throw SocketClosed();
  }
  elle::Buffer res;
  res.size(sz);
  memcpy(res.mutable_contents(), _read_buffer.contents(), sz);
  memmove(_read_buffer.contents(), _read_buffer.contents() + sz,
          _read_buffer.size() - sz);
  _read_buffer.size(_read_buffer.size() - sz);
  return std::move(res);
}

elle::Buffer UTPSocket::read_some(size_t sz, DurationOpt opt)
{
  using namespace boost::posix_time;
  if (!_open)
    throw SocketClosed();
  ELLE_DEBUG("read_some");
  ptime start = microsec_clock::universal_time();
  while (_read_buffer.empty())
  {
    ELLE_DEBUG("read_some wait");
    _read_barrier.close();
    Duration elapsed = microsec_clock::universal_time() - start;
    if (opt && *opt < elapsed)
      throw TimeOut();
    _read_barrier.wait(opt? elapsed - *opt: opt);
    ELLE_DEBUG("read_some wake");
    if (!_open)
      throw SocketClosed();
  }
  if (_read_buffer.size() <= sz)
  {
    elle::Buffer res;
    std::swap(res, _read_buffer);
    return std::move(res);
  }
  elle::Buffer res;
  res.size(sz);
  memcpy(res.mutable_contents(), _read_buffer.contents(), sz);
  memmove(_read_buffer.contents(), _read_buffer.contents() + sz,
          _read_buffer.size() - sz);
  _read_buffer.size(_read_buffer.size() - sz);
  return std::move(res);
}

void UTPServer::listen(int port)
{
  listen(EndPoint(boost::asio::ip::address(), port));
}

void UTPServer::listen(EndPoint const& ep)
{
  _socket = elle::make_unique<UDPSocket>();
  _socket->close();
  _socket->bind(ep);
  _listener.reset( new Thread("listener " + std::to_string(local_endpoint().port()), [this] {
      elle::Buffer buf;
      while (true)
      {
        buf.size(5000);
        EndPoint source;
        ELLE_DEBUG("Receive from");
        int sz = 0;
        try
        {
          if (!_socket->socket()->is_open())
          {
            ELLE_DEBUG("Socket closed, exiting");
            return;
          }
          sz = _socket->receive_from(Buffer(buf.mutable_contents(), buf.size()),
                                    source);
          buf.size(sz);
          auto* raw = source.data();
          ELLE_DEBUG("process_udp %s", sz);
          utp_process_udp(ctx, buf.contents(), sz, raw, source.size());
          utp_issue_deferred_acks(ctx);

        }
        catch (reactor::Terminate const&)
        {
          throw;
        }
        catch (std::exception const& e)
        {
          ELLE_WARN("listener exception %s", e.what());
          return;
        }
      }
  }));
  _checker.reset(new Thread("checker", [this] {
      try
      {
        while (true)
        {
          ELLE_DEBUG("check timeout");
          utp_check_timeouts(ctx);
          ELLE_DEBUG("check timeout sleeping");
          reactor::sleep(50_ms);
        }
      }
      catch(std::exception const& e)
      {
        ELLE_DEBUG("exiting: %s", e.what());
        throw;
      }
  }));
}
}}