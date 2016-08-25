#include <reactor/network/utp-socket.hh>

#include <utp.h>

#include <elle/log.hh>

#include <reactor/Barrier.hh>
#include <reactor/MultiLockBarrier.hh>
#include <reactor/exception.hh>
#include <reactor/mutex.hh>
#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/fwd.hh>
#include <reactor/network/utp-server.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

ELLE_LOG_COMPONENT("reactor.network.UTPSocket");

namespace reactor
{
  namespace network
  {
    /*-------------.
    | StreamBuffer |
    `-------------*/

    namespace
    {
      class StreamBuffer
        : public elle::DynamicStreamBuffer
      {
      public:
        StreamBuffer(UTPSocket* socket)
          : DynamicStreamBuffer(65536)
          , _socket(socket)
        {}

        Size
        read(char* buffer, Size size) override
        {
          elle::Buffer buf = this->_socket->read_some(size);
          memcpy(buffer, buf.contents(), buf.size());
          return buf.size();
        }

        void
        write(char* buffer, Size size) override
        {
          this->_socket->write(elle::ConstWeakBuffer(buffer, size));
        }

        UTPSocket* _socket;
      };
    }

    /*-----.
    | Impl |
    `-----*/

    class UTPSocket::Impl
    {
    public:
      friend class UTPSocket;

      Impl(UTPServer& server, utp_socket* socket, bool open);

      ELLE_ATTRIBUTE(utp_socket*, socket);
      ELLE_ATTRIBUTE(elle::Buffer, read_buffer);
      ELLE_ATTRIBUTE(Barrier, read_barrier);
      ELLE_ATTRIBUTE(Barrier, write_barrier);
      ELLE_ATTRIBUTE(Mutex, write_mutex);
      ELLE_ATTRIBUTE(Barrier, connect_barrier);
      ELLE_ATTRIBUTE(Barrier, destroyed_barrier);
      ELLE_ATTRIBUTE_R(UTPServer&, server);
      ELLE_ATTRIBUTE(elle::ConstWeakBuffer, write);
      ELLE_ATTRIBUTE(MultiLockBarrier, pending_operations);
      ELLE_ATTRIBUTE(int, write_pos);
      ELLE_ATTRIBUTE(bool, open);
      ELLE_ATTRIBUTE(bool, closing);
    };

    /*-------------.
    | Construction |
    `-------------*/

    UTPSocket::UTPSocket(UTPServer& server, utp_socket* socket, bool open)
      : IOStream(new StreamBuffer(this))
      , _impl(elle::make_unique<Impl>(server, socket, open))
    {
      utp_set_userdata(this->_impl->_socket, this);
      if (open)
      {
        this->_impl->_write_barrier.open();
        ELLE_DEBUG("snd %s recv %s",
                   utp_getsockopt(this->_impl->_socket, UTP_SNDBUF),
                   utp_getsockopt(this->_impl->_socket, UTP_RCVBUF));
      }
      else
        this->_impl->_destroyed_barrier.open();
    }

    UTPSocket::Impl::Impl(UTPServer& server, utp_socket* socket, bool open)
      : _socket(socket) // socket first because it is used when printing this
      , _read_barrier(elle::sprintf("%s read", this))
      , _write_barrier(elle::sprintf("%s write", this))
      , _write_mutex()
      , _connect_barrier(elle::sprintf("%s connection", this))
      , _destroyed_barrier(elle::sprintf("%s desroyed", this))
      , _server(server)
      , _pending_operations(elle::sprintf("%s pending operations", this))
      , _write_pos(0)
      , _open(open)
      , _closing(false)
    {}

    UTPSocket::UTPSocket(UTPServer& server)
      : UTPSocket(server, utp_create_socket(server.ctx), false)
    {
      this->_impl->_destroyed_barrier.open();
    }

    UTPSocket::UTPSocket(UTPServer& server, std::string const& host, int port)
      : UTPSocket(server, utp_create_socket(server.ctx), false)
    {
      connect(host, port);
    }

    UTPSocket::~UTPSocket()
    {
      ELLE_DEBUG("%s: ~UTPSocket", this);
      try
      {
        this->on_close();
        reactor::wait(this->_impl->_pending_operations);
        ELLE_DEBUG("%s from %s: waiting for destroyed...",
                   this, &this->_impl->_server);
        if (!reactor::wait(this->_impl->_destroyed_barrier, 0_sec))
        {
          if (!this->_impl->_server._socket || !this->_impl->_server._checker
             || this->_impl->_server._checker->done())
            ELLE_WARN("%s: server %s was destroyed before us",
                      this, this->_impl->_server);
          else if (!reactor::wait(this->_impl->_destroyed_barrier, 90_sec))
          {
            ELLE_WARN("%s: timeout waiting for DESTROYED state", this);
          }
        }
      }
      catch (std::exception const& e)
      {
        ELLE_ERR("%s: losing exception in UTP socket destructor: %s", this, e);
        ELLE_ERR("%s", elle::Backtrace::current());
      }
      destroyed();
      ELLE_DEBUG("%s: ~UTPSocket finished", this);
    }

    /*----------.
    | Callbacks |
    `----------*/

    void
    UTPSocket::on_read(elle::ConstWeakBuffer const& data)
    {
      this->_impl->_read_buffer.append(data.contents(), data.size());
      utp_read_drained(this->_impl->_socket);
      this->_read();
    }

    void
    UTPSocket::write_cont()
    {
      if (this->_impl->_write.size())
      {
        unsigned char* data =
          const_cast<unsigned char*>(this->_impl->_write.contents());
        int sz = this->_impl->_write.size();
        while (this->_impl->_write_pos < sz)
        {
          ELLE_DEBUG("%s: writing at offset %s/%s",
                     this, this->_impl->_write_pos, sz);
          ssize_t len = utp_write(this->_impl->_socket,
                                  data + this->_impl->_write_pos,
                                  sz - this->_impl->_write_pos);
          if (!len)
          {
            ELLE_DEBUG("from status: write buffer full");
            break;
          }
          this->_impl->_write_pos += len;
        }
        if (this->_impl->_write_pos == sz)
          this->_impl->_write_barrier.open();
      }
    }

    void
    UTPSocket::on_connect()
    {
      this->_impl->_open = true;
      this->_impl->_connect_barrier.open();
      this->_impl->_write_barrier.open();
    }

    void
    UTPSocket::destroyed()
    {
      this->_impl->_read_barrier.open();
      this->_impl->_write_barrier.open();
      this->_impl->_connect_barrier.open();
      this->_impl->_destroyed_barrier.open();
      if (this->_impl->_socket)
        utp_set_userdata(this->_impl->_socket, nullptr);
      this->_impl->_socket = nullptr;
    }

    void
    UTPSocket::on_close()
    {
      if (this->_impl->_closing)
        return;
      this->_impl->_closing = true;
      if (!this->_impl->_socket)
        return;
      //if (_open)
      ELLE_DEBUG("%s: closing underlying socket", this);
      utp_close(this->_impl->_socket);
      this->_impl->_open = false;
      this->_impl->_read_barrier.open();
      this->_impl->_write_barrier.open();
      this->_impl->_connect_barrier.open();
    }

    void
    UTPSocket::close()
    {
      this->on_close();
    }

    void UTPSocket::connect(std::string const& id,
                            std::vector<EndPoint> const& endpoints,
                            DurationOpt timeout)
    {
      ELLE_TRACE("Contacting %s at %s", id, endpoints);
      EndPoint res =
        this->_impl->_server._socket->contact(id, endpoints, timeout);
      ELLE_TRACE("Got contact at %s", res);
      connect(res.address().to_string(), res.port());
    }

    void
    UTPSocket::connect(std::string const& host, int port)
    {
      ELLE_TRACE_SCOPE("%s: connect to %s:%s", *this, host, port);
      auto lock = this->_impl->_pending_operations.lock();
      struct addrinfo* ai = nullptr;
      addrinfo hints;
      memset(&hints, 0, sizeof(hints));
      hints.ai_family = AF_INET;
      hints.ai_socktype = SOCK_DGRAM;
      hints.ai_protocol = IPPROTO_UDP;
      int res = getaddrinfo(host.c_str(), std::to_string(port).c_str(),
                            &hints, &ai);
      // IPv4 failed, try IPv6.
      if (res)
      {
        if (ai)
          freeaddrinfo(ai);
        hints.ai_family = AF_INET6;
        res = getaddrinfo(host.c_str(), std::to_string(port).c_str(),
                          &hints, &ai);
      }
      if (res)
        throw elle::Error("Failed to resolve " + host);
      this->_impl->_destroyed_barrier.close();
      utp_connect(this->_impl->_socket, ai->ai_addr, ai->ai_addrlen);
      freeaddrinfo(ai);
      this->_impl->_connect_barrier.wait();
      if (!this->_impl->_open)
        throw SocketClosed();
    }

    void
    UTPSocket::write(elle::ConstWeakBuffer const& buf, DurationOpt opt)
    {
      ELLE_DEBUG("write %s", buf.size());
      using namespace boost::posix_time;
      if (!this->_impl->_open)
        throw SocketClosed();
      auto lock = this->_impl->_pending_operations.lock();
      ptime start = microsec_clock::universal_time();
      Lock l(this->_impl->_write_mutex);
      unsigned char* data = const_cast<unsigned char*>(buf.contents());
      int sz = buf.size();
      this->_impl->_write = buf;
      this->_impl->_write_pos = 0;
      while (this->_impl->_write_pos < sz)
      {
        ssize_t len = utp_write(this->_impl->_socket,
                                data + this->_impl->_write_pos,
                                sz - this->_impl->_write_pos);
        if (!len)
        {
          ELLE_DEBUG("write buffer full");
          this->_impl->_write_barrier.close();
          Duration elapsed = microsec_clock::universal_time() - start;
          if (opt && *opt < elapsed)
            throw TimeOut();
          this->_impl->_write_barrier.wait(opt ? elapsed - *opt : opt);
          ELLE_DEBUG("write woken up");
          if (!this->_impl->_open)
            throw SocketClosed();
          continue;
        }
        this->_impl->_write_pos += len;
      }
      this->_impl->_write_pos = 0;
      this->_impl->_write = {};
    }

    void
    UTPSocket::_read()
    {
      this->_impl->_read_barrier.open();
    }

    void
    UTPSocket::stats()
    {
      utp_socket_stats* st = utp_get_stats(this->_impl->_socket);
      if (st == nullptr)
        return;
      std::cerr << "recv " << st->nbytes_recv
                << "\nsent " << st->nbytes_xmit
                << "\nrexmit " << st->rexmit
                << "\nfastrexmit " << st->fastrexmit
                << "\nnxmit " << st->nxmit
                << "\nnrecv" << st->nrecv
                << "\nnduprect " << st->nduprecv
                <<"\nmtu " << st->mtu_guess << std::endl;
    }

    elle::Buffer
    UTPSocket::read_until(std::string const& delimiter, DurationOpt opt)
    {
      using namespace boost::posix_time;
      if (!this->_impl->_open)
        throw SocketClosed();
      auto lock = this->_impl->_pending_operations.lock();
      ptime start = microsec_clock::universal_time();
      while (true)
      {
        size_t p = this->_impl->_read_buffer.string().find(delimiter);
        if (p != std::string::npos)
          return read(p + delimiter.length());
        this->_impl->_read_barrier.close();
        Duration elapsed = microsec_clock::universal_time() - start;
        if (opt && *opt < elapsed)
          throw TimeOut();
        this->_impl->_read_barrier.wait(opt ? elapsed - *opt : opt);
        if (!this->_impl->_open)
          throw SocketClosed();
      }
    }

    elle::Buffer
    UTPSocket::read(size_t sz, DurationOpt opt)
    {
      ELLE_TRACE_SCOPE("%s: read up to %s bytes", this, sz);
      using namespace boost::posix_time;
      if (!this->_impl->_open)
        throw SocketClosed();
      auto lock = this->_impl->_pending_operations.lock();
      ptime start = microsec_clock::universal_time();
      while (this->_impl->_read_buffer.size() < sz)
      {
        ELLE_DEBUG("read wait %s", this->_impl->_read_buffer.size());
        this->_impl->_read_barrier.close();
        Duration elapsed = microsec_clock::universal_time() - start;
        if (opt && *opt < elapsed)
          throw TimeOut();
        this->_impl->_read_barrier.wait(opt ? elapsed - *opt : opt);
        ELLE_DEBUG("read wake %s", this->_impl->_read_buffer.size());
        if (!this->_impl->_open)
          throw SocketClosed();
      }
      elle::Buffer res;
      res.size(sz);
      memcpy(res.mutable_contents(), this->_impl->_read_buffer.contents(), sz);
      memmove(this->_impl->_read_buffer.contents(),
              this->_impl->_read_buffer.contents() + sz,
              this->_impl->_read_buffer.size() - sz);
      this->_impl->_read_buffer.size(this->_impl->_read_buffer.size() - sz);
      return res;
    }

    elle::Buffer
    UTPSocket::read_some(size_t sz, DurationOpt opt)
    {
      using namespace boost::posix_time;
      if (!this->_impl->_open)
        throw SocketClosed();
      auto lock = this->_impl->_pending_operations.lock();
      ELLE_DEBUG("read_some");
      ptime start = microsec_clock::universal_time();
      while (this->_impl->_read_buffer.empty())
      {
        ELLE_DEBUG("read_some wait");
        this->_impl->_read_barrier.close();
        Duration elapsed = microsec_clock::universal_time() - start;
        if (opt && *opt < elapsed)
          throw TimeOut();
        this->_impl->_read_barrier.wait(opt ? elapsed - *opt : opt);
        ELLE_DEBUG("read_some wake");
        if (!this->_impl->_open)
          throw SocketClosed();
      }
      if (this->_impl->_read_buffer.size() <= sz)
      {
        elle::Buffer res;
        std::swap(res, this->_impl->_read_buffer);
        return res;
      }
      elle::Buffer res;
      res.size(sz);
      memcpy(res.mutable_contents(), this->_impl->_read_buffer.contents(), sz);
      memmove(this->_impl->_read_buffer.contents(),
              this->_impl->_read_buffer.contents() + sz,
              this->_impl->_read_buffer.size() - sz);
      this->_impl->_read_buffer.size(this->_impl->_read_buffer.size() - sz);
      return res;
    }

    UTPSocket::EndPoint
    UTPSocket::peer() const
    {
      using namespace boost::asio::ip;
      struct sockaddr_in addr;
      socklen_t addrlen = sizeof(addr);
      if (!this->_impl->_socket ||
          utp_getpeername(this->_impl->_socket,
                          (sockaddr*)&addr, &addrlen) == -1)
        return EndPoint(boost::asio::ip::address::from_string("0.0.0.0"), 0);
      if (addr.sin_family == AF_INET)
      {
        return EndPoint(address_v4(ntohl(addr.sin_addr.s_addr)),
                        ntohs(addr.sin_port));
      }
      else if (addr.sin_family == AF_INET6)
      {
        struct sockaddr_in6* addr6 = (struct sockaddr_in6*)&addr;
        std::array<unsigned char, 16> addr_bytes {{0}};
        memcpy(addr_bytes.data(), addr6->sin6_addr.s6_addr, 16);
        return EndPoint(address_v6(addr_bytes), ntohs(addr6->sin6_port));
      }
      else
      {
        throw elle::Error(
          elle::sprintf("unknown protocol %s", addr.sin_family));
      }
    }

    void
    UTPSocket::print(std::ostream& output) const
    {
      elle::fprintf(output, "%s(%s)", elle::type_info(*this), this->peer());
    }
  }
}
