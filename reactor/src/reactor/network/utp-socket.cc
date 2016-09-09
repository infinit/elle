#include <reactor/network/utp-socket-impl.hh>

#include <utp.h>

#include <elle/finally.hh>
#include <elle/log.hh>

#include <reactor/exception.hh>
#include <reactor/mutex.hh>
#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/fwd.hh>
#include <reactor/network/utp-server-impl.hh>
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

    /*-------------.
    | Construction |
    `-------------*/

    UTPSocket::UTPSocket(std::unique_ptr<Impl> impl)
      : IOStream(new StreamBuffer(this))
      , _impl(std::move(impl))
    {}

    UTPSocket::Impl::Impl(
      std::weak_ptr<UTPServer::Impl> server, utp_socket* socket, bool open)
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
    {
      utp_set_userdata(this->_socket, this);
    }

    UTPSocket::UTPSocket(UTPServer& server)
      : UTPSocket(elle::make_unique<Impl>(server._impl,
                                          utp_create_socket(server._impl->_ctx),
                                          false))
    {
      this->_impl->_destroyed_barrier.open();
    }

    UTPSocket::UTPSocket(UTPServer& server, std::string const& host, int port)
      : UTPSocket(elle::make_unique<Impl>(server._impl,
                                          utp_create_socket(server._impl->_ctx),
                                          false))
    {
      connect(host, port);
    }

    UTPSocket::~UTPSocket()
    {
      ELLE_TRACE_SCOPE("%s: destruct", this);
      auto name = elle::sprintf("%s shutdown", this);
      this->_impl->on_close();
      reactor::run_later(
        std::move(name),
        [impl = std::shared_ptr<Impl>(this->_impl.release())]
        {
          try
          {
            reactor::wait(impl->_pending_operations);
            ELLE_DEBUG("%s from %s: waiting for destroyed ...",
                       impl, &impl->_server);
            if (!impl->_destroyed_barrier.opened())
            {
              auto server = impl->_server.lock();
              if (server &&
                  server->_socket &&
                  server->_checker &&
                  !server->_checker->done())
              {
                if (!reactor::wait(impl->_destroyed_barrier, 90_sec))
                  ELLE_WARN("%s: timeout on UTP shutdown", impl);
              }
              else
                ELLE_WARN("%s: UTP server was destroyed before us", impl);
            }
          }
          catch (elle::Error const& e)
          {
            ELLE_ERR("%s: exception in UTP shutdown: %s", impl, e);
          }
        });
    }

    UTPSocket::Impl::~Impl()
    {
      this->_destroyed();
    }

    /*----------.
    | Callbacks |
    `----------*/

    void
    UTPSocket::Impl::on_connect()
    {
      this->_open = true;
      this->_connect_barrier.open();
      this->_write_barrier.open();
    }

    void
    UTPSocket::Impl::on_close()
    {
      auto server = this->_server.lock();
      if (this->_socket && server)
      {
        ELLE_DEBUG("%s: closing underlying socket", this);
        utp_close(this->_socket);
      }
      if (this->_closing)
        return;
      this->_closing = true;
      if (!this->_socket)
        return;
      this->_open = false;
      this->_read_barrier.open();
      this->_write_barrier.open();
      this->_connect_barrier.open();
    }

    void
    UTPSocket::Impl::on_read(elle::ConstWeakBuffer const& data)
    {
      this->_read_buffer.append(data.contents(), data.size());
      utp_read_drained(this->_socket);
      this->_read();
    }

    /*-----------.
    | Operations |
    `-----------*/

    void
    UTPSocket::Impl::_destroyed()
    {
      this->_read_barrier.open();
      this->_write_barrier.open();
      this->_connect_barrier.open();
      this->_destroyed_barrier.open();
      if (this->_socket)
        utp_set_userdata(this->_socket, nullptr);
      this->_socket = nullptr;
    }

    void
    UTPSocket::Impl::_read()
    {
      this->_read_barrier.open();
    }

    void
    UTPSocket::Impl::_write_cont()
    {
      if (this->_write.size())
      {
        unsigned char* data =
          const_cast<unsigned char*>(this->_write.contents());
        int sz = this->_write.size();
        while (this->_write_pos < sz)
        {
          ELLE_DEBUG("%s: writing at offset %s/%s",
                     this, this->_write_pos, sz);
          ssize_t len = utp_write(this->_socket,
                                  data + this->_write_pos,
                                  sz - this->_write_pos);
          if (!len)
          {
            ELLE_DEBUG("from status: write buffer full");
            break;
          }
          this->_write_pos += len;
        }
        if (this->_write_pos == sz)
          this->_write_barrier.open();
      }
    }

    void
    UTPSocket::close()
    {
      this->_impl->on_close();
    }

    void UTPSocket::connect(std::string const& id,
                            std::vector<EndPoint> const& endpoints,
                            DurationOpt timeout)
    {
      if (auto server = this->_impl->_server.lock())
      {
        ELLE_TRACE_SCOPE("%s: connect to %s with id %s", endpoints, id);
        EndPoint res = server->_socket->contact(id, endpoints, timeout);
        ELLE_DEBUG("got contact: %s", res);
        this->connect(res.address().to_string(), res.port());
      }
      else
        elle::err("unable to connect: UTP server was destroyed");
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
        throw ConnectionRefused();
    }

    void
    UTPSocket::write(elle::ConstWeakBuffer const& buf, DurationOpt opt)
    {
      ELLE_DEBUG("write %s", buf.size());
      using namespace boost::posix_time;
      if (!this->_impl->_open)
        throw ConnectionClosed();
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
            throw ConnectionClosed();
          continue;
        }
        this->_impl->_write_pos += len;
      }
      this->_impl->_write_pos = 0;
      this->_impl->_write = {};
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
        throw ConnectionClosed();
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
          throw ConnectionClosed();
      }
    }

    elle::Buffer
    UTPSocket::read(size_t sz, DurationOpt opt)
    {
      ELLE_TRACE_SCOPE("%s: read up to %s bytes", this, sz);
      using namespace boost::posix_time;
      if (!this->_impl->_open)
        throw ConnectionClosed();
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
          throw ConnectionClosed();
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
        throw ConnectionClosed();
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
          throw ConnectionClosed();
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

    /*-----------.
    | Attributes |
    `-----------*/

    UTPSocket::EndPoint
    UTPSocket::peer() const
    {
      return this->_impl->peer();
    }

    UTPSocket::EndPoint
    UTPSocket::Impl::peer() const
    {
      using namespace boost::asio::ip;
      struct sockaddr_in addr;
      socklen_t addrlen = sizeof(addr);
      if (!this->_socket ||
          utp_getpeername(this->_socket, (sockaddr*)&addr, &addrlen) == -1)
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
        elle::err("unknown protocol %s", addr.sin_family);
    }

    /*----------.
    | Printable |
    `----------*/

    void
    UTPSocket::print(std::ostream& output) const
    {
      output << *this->_impl;
    }

    std::ostream&
    operator <<(std::ostream& output, UTPSocket::Impl const& impl)
    {
      elle::fprintf(output, "UTPSocket(%s)", impl.peer());
      return output;
    }
  }
}
