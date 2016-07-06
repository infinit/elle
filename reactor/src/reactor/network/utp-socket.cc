#include <reactor/network/utp-socket.hh>

#include <utp.h>

#include <elle/log.hh>

#include <reactor/exception.hh>
#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/utp-server.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

ELLE_LOG_COMPONENT("reactor.network.utp");

namespace reactor
{
  namespace network
  {
    void
    UTPSocket::on_read(elle::ConstWeakBuffer const& data)
    {
      this->_read_buffer.append(data.contents(), data.size());
      utp_read_drained(this->_socket);
      this->_read();
    }

    void
    UTPSocket::write_cont()
    {
      if (this->_write.size())
      {
        unsigned char* data =
          const_cast<unsigned char*>(this->_write.contents());
        int sz = this->_write.size();
        while (this->_write_pos < sz)
        {
          ELLE_DEBUG("%s: writing at offset %s/%s", this, this->_write_pos, sz);
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
    UTPSocket::on_connect()
    {
      this->_open = true;
      this->_connect_barrier.open();
      this->_write_barrier.open();
    }

    UTPSocket::UTPSocket(UTPServer& server)
      : UTPSocket(server, utp_create_socket(server.ctx), false)
    {
      this->_destroyed_barrier.open();
    }

    UTPSocket::UTPSocket(UTPServer& server, std::string const& host, int port)
      : UTPSocket(server, utp_create_socket(server.ctx), false)
    {
      connect(host, port);
    }

    UTPSocket::~UTPSocket()
    {
      ELLE_DEBUG("%s: ~UTPSocket", this);
      this->on_close();
      reactor::wait(_pending_operations);
      reactor::wait(_destroyed_barrier);
      destroyed();
      ELLE_DEBUG("~UTPSocket finished");
    }

    void
    UTPSocket::destroyed()
    {
      this->_read_barrier.open();
      this->_write_barrier.open();
      this->_connect_barrier.open();
      this->_destroyed_barrier.open();
      if (this->_socket)
        utp_set_userdata(_socket, nullptr);
      this->_socket = nullptr;
    }

    void
    UTPSocket::on_close()
    {
      if (this->_closing)
        return;
      this->_closing = true;
      if (!this->_socket)
        return;
      //if (_open)
      ELLE_DEBUG("%s: closing underlying socket", this);
      utp_close(this->_socket);
      this->_open = false;
      this->_read_barrier.open();
      this->_write_barrier.open();
      this->_connect_barrier.open();
    }

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

    UTPSocket::UTPSocket(UTPServer& server, utp_socket* socket, bool open)
      : IOStream(new StreamBuffer(this))
      , _read_barrier("utp socket read")
      , _write_barrier("utp socket write")
      , _write_mutex()
      , _connect_barrier("connect barrier")
      , _server(server)
      , _socket(socket)
      , _open(open)
      , _closing(false)
    {
      utp_set_userdata(this->_socket, this);
      if (open)
      {
        this->_write_barrier.open();
        ELLE_DEBUG("snd %s recv %s", utp_getsockopt(this->_socket, UTP_SNDBUF),
          utp_getsockopt(this->_socket, UTP_RCVBUF));
      }
      else
        this->_destroyed_barrier.open();
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
      EndPoint res = this->_server._socket->contact(id, endpoints, timeout);
      ELLE_TRACE("Got contact at %s", res);
      connect(res.address().to_string(), res.port());
    }

    void
    UTPSocket::connect(std::string const& host, int port)
    {
      auto lock = this->_pending_operations.lock();
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
      this->_destroyed_barrier.close();
      utp_connect(this->_socket, ai->ai_addr, ai->ai_addrlen);
      freeaddrinfo(ai);
      ELLE_DEBUG("waiting for connect...");
      this->_connect_barrier.wait();
      ELLE_DEBUG("connected");
      if (!this->_open)
        throw SocketClosed();
    }

    void
    UTPSocket::write(elle::ConstWeakBuffer const& buf, DurationOpt opt)
    {
      ELLE_DEBUG("write %s", buf.size());
      using namespace boost::posix_time;
      if (!this->_open)
        throw SocketClosed();
      auto lock = this->_pending_operations.lock();
      ptime start = microsec_clock::universal_time();
      Lock l(this->_write_mutex);
      unsigned char* data = const_cast<unsigned char*>(buf.contents());
      int sz = buf.size();
      this->_write = buf;
      this->_write_pos = 0;
      while (this->_write_pos < sz)
      {
        ssize_t len = utp_write(this->_socket,
                                data + this->_write_pos,
                                sz - this->_write_pos);
        if (!len)
        {
          ELLE_DEBUG("write buffer full");
          this->_write_barrier.close();
          Duration elapsed = microsec_clock::universal_time() - start;
          if (opt && *opt < elapsed)
            throw TimeOut();
          this->_write_barrier.wait(opt ? elapsed - *opt : opt);
          ELLE_DEBUG("write woken up");
          if (!this->_open)
            throw SocketClosed();
          continue;
        }
        this->_write_pos += len;
      }
      this->_write_pos = 0;
      this->_write = {};
    }

    void
    UTPSocket::_read()
    {
      this->_read_barrier.open();
    }

    void
    UTPSocket::stats()
    {
      utp_socket_stats* st = utp_get_stats(this->_socket);
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
      if (!this->_open)
        throw SocketClosed();
      auto lock = this->_pending_operations.lock();
      ptime start = microsec_clock::universal_time();
      while (true)
      {
        size_t p = this->_read_buffer.string().find(delimiter);
        if (p != std::string::npos)
          return read(p + delimiter.length());
        this->_read_barrier.close();
        Duration elapsed = microsec_clock::universal_time() - start;
        if (opt && *opt < elapsed)
          throw TimeOut();
        this->_read_barrier.wait(opt ? elapsed - *opt : opt);
        if (!this->_open)
          throw SocketClosed();
      }
    }

    elle::Buffer
    UTPSocket::read(size_t sz, DurationOpt opt)
    {
      using namespace boost::posix_time;
      if (!this->_open)
        throw SocketClosed();
      auto lock = this->_pending_operations.lock();
      ELLE_DEBUG("read");
      ptime start = microsec_clock::universal_time();
      while (this->_read_buffer.size() < sz)
      {
        ELLE_DEBUG("read wait %s", this->_read_buffer.size());
        this->_read_barrier.close();
        Duration elapsed = microsec_clock::universal_time() - start;
        if (opt && *opt < elapsed)
          throw TimeOut();
        this->_read_barrier.wait(opt ? elapsed - *opt : opt);
        ELLE_DEBUG("read wake %s", this->_read_buffer.size());
        if (!this->_open)
          throw SocketClosed();
      }
      elle::Buffer res;
      res.size(sz);
      memcpy(res.mutable_contents(), this->_read_buffer.contents(), sz);
      memmove(this->_read_buffer.contents(), this->_read_buffer.contents() + sz,
              this->_read_buffer.size() - sz);
      this->_read_buffer.size(this->_read_buffer.size() - sz);
      return res;
    }

    elle::Buffer
    UTPSocket::read_some(size_t sz, DurationOpt opt)
    {
      using namespace boost::posix_time;
      if (!this->_open)
        throw SocketClosed();
      auto lock = this->_pending_operations.lock();
      ELLE_DEBUG("read_some");
      ptime start = microsec_clock::universal_time();
      while (this->_read_buffer.empty())
      {
        ELLE_DEBUG("read_some wait");
        this->_read_barrier.close();
        Duration elapsed = microsec_clock::universal_time() - start;
        if (opt && *opt < elapsed)
          throw TimeOut();
        this->_read_barrier.wait(opt ? elapsed - *opt : opt);
        ELLE_DEBUG("read_some wake");
        if (!this->_open)
          throw SocketClosed();
      }
      if (this->_read_buffer.size() <= sz)
      {
        elle::Buffer res;
        std::swap(res, this->_read_buffer);
        return res;
      }
      elle::Buffer res;
      res.size(sz);
      memcpy(res.mutable_contents(), this->_read_buffer.contents(), sz);
      memmove(this->_read_buffer.contents(), this->_read_buffer.contents() + sz,
              this->_read_buffer.size() - sz);
      this->_read_buffer.size(this->_read_buffer.size() - sz);
      return res;
    }

    UTPSocket::EndPoint
    UTPSocket::peer()
    {
      using namespace boost::asio::ip;
      struct sockaddr_in addr;
      socklen_t addrlen = sizeof(addr);
      utp_getpeername(this->_socket, (sockaddr*)&addr, &addrlen);
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
  }
}
