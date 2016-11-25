#include <reactor/network/utp-server.hh>

#ifdef INFINIT_LINUX
# include <sys/time.h> // timespec
# include <linux/errqueue.h>
# include <netinet/ip_icmp.h>
#endif

#ifdef INFINIT_MACOSX
# include <netinet/in_systm.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <sys/socket.h>
#endif

#include <utp.h>

#include <elle/Buffer.hh>
#include <elle/log.hh>

#include <reactor/network/buffer.hh> // FIXME: replace with elle::WeakBuffer
#include <reactor/scheduler.hh>
#include <reactor/network/utp-server-impl.hh>
#include <reactor/network/utp-socket-impl.hh>

ELLE_LOG_COMPONENT("reactor.network.UTPServer");

namespace reactor
{
  namespace network
  {
    static
    uint64
    on_firewall(utp_callback_arguments *a)
    {
      return 0;
    }

    static inline
    UTPSocket::Impl*
    get(utp_callback_arguments* args)
    {
      return reinterpret_cast<UTPSocket::Impl*>(utp_get_userdata(args->socket));
    }

    static inline
    UTPServer::Impl*
    get_server(utp_callback_arguments* args)
    {
      return reinterpret_cast<UTPServer::Impl*>(
        utp_context_get_userdata(args->context));
    }

    static
    uint64
    on_sendto(utp_callback_arguments* args)
    {
      UTPServer::EndPoint ep;
      sockaddr_in *sin = (sockaddr_in*)args->address;
      if (sin->sin_family == AF_INET)
      {
        ep = UTPServer::EndPoint(
          boost::asio::ip::address_v4(ntohl(sin->sin_addr.s_addr)),
          ntohs(sin->sin_port));
      }
      else if (sin->sin_family == AF_INET6)
      {
        sockaddr_in6 *sin = (sockaddr_in6*)args->address;
        std::array<unsigned char, 16> addr {{0}};
        memcpy(addr.data(), sin->sin6_addr.s6_addr, 16);
        ep = UTPServer::EndPoint(boost::asio::ip::address_v6(addr),
                                 ntohs(sin->sin6_port));
      }
      else
      {
        elle::err("unknown protocol %s", sin->sin_family);
      }
      auto server = get_server(args);
      ELLE_ASSERT(server);
      Buffer buf(args->buf, args->len);
      elle::Buffer copy;
      if (server->xorify())
      {
        copy.append(args->buf, args->len);
        for (unsigned int i = 0; i < args->len; ++i)
          copy[i] ^= server->xorify();
        buf = Buffer(copy.contents(), copy.size());
      }
      server->send_to(buf, ep);
      return 0;
    }

    static
    uint64
    on_read(utp_callback_arguments* args)
    {
      ELLE_DEBUG("on_read");
      auto s = get(args);
      if (s)
        s->on_read(elle::ConstWeakBuffer(args->buf, args->len));
      return 0;
    }

    static
    uint64
    on_error(utp_callback_arguments* args)
    {
      ELLE_DEBUG("on_error %s", utp_error_code_names[args->error_code]);
      auto s = get(args);
      if (s)
        s->on_close();
      return 0;
    }

    static
    uint64
    on_state_change(utp_callback_arguments* args)
    {
      auto s = get(args);
      ELLE_DEBUG("on_state_change %s on %s", utp_state_names[args->state], s);
      if (s)
        switch (args->state)
          {
          case UTP_STATE_CONNECT:
          case UTP_STATE_WRITABLE:
            s->_write_cont();
            //s->_write_barrier.open();
            break;
          case UTP_STATE_EOF:
            s->on_close();
            break;
          case UTP_STATE_DESTROYING:
            s->_destroyed();
            break;
          }
      return 0;
    }

    static
    uint64
    on_accept(utp_callback_arguments* args)
    {
      ELLE_DEBUG("on_accept");
      auto server = get_server(args);
      server->on_accept(args->socket);
      return 0;
    }

    static
    uint64
    on_connect(utp_callback_arguments* args)
    {
      ELLE_DEBUG("on_connect");
      auto s = get(args);
      if (!s)
        utp_close(args->socket);
      else
        s->on_connect();
      return 0;
    }

    static
    uint64
    on_log(utp_callback_arguments* args)
    {
      ELLE_DEBUG("utp: %s", args->buf);
      return 0;
    }

    /*-------------.
    | Construction |
    `-------------*/

    UTPServer::UTPServer()
      : _impl(std::make_shared<UTPServer::Impl>())
    {}

    UTPServer::~UTPServer()
    {}

    /*-----------.
    | Attributes |
    `-----------*/

    unsigned char
    UTPServer::xorify() const
    {
      return this->_impl->_xorify;
    }

    void
    UTPServer::xorify(unsigned char v)
    {
      this->_impl->_xorify = v;
    }

    /*-----------.
    | Networking |
    `-----------*/

    std::unique_ptr<UTPSocket>
    UTPServer::accept()
    {
      return this->_impl->accept();
    }

    UTPServer::EndPoint
    UTPServer::local_endpoint()
    {
      auto ep = this->_impl->_socket->local_endpoint();
      return EndPoint(ep.address(), ep.port());
    }

    bool
    UTPServer::rdv_connected() const
    {
      return this->_impl->_socket->rdv_connected();
    }

    void
    UTPServer::listen(int port, bool ipv6)
    {
      if (ipv6)
        this->listen(EndPoint(boost::asio::ip::address_v6::any(), port));
      else
        this->listen(EndPoint(boost::asio::ip::address_v4::any(), port));
    }

    void
    UTPServer::listen(boost::asio::ip::address host, int port, bool enable_ipv6)
    {
      if (enable_ipv6 && host.is_v4())
        host = boost::asio::ip::address_v6::v4_mapped(host.to_v4());
      this->listen(EndPoint(host, port));
    }

    void
    UTPServer::listen(EndPoint const& ep)
    {
      this->_impl->listen(ep);
    }


    void
    UTPServer::rdv_connect(
      std::string const& id, std::string const& address, DurationOpt timeout)
    {
      int port = 7890;
      std::string host = address;
      auto p = host.find_first_of(':');
      if (p != host.npos)
      {
        port = std::stoi(host.substr(p+1));
        host = host.substr(0, p);
      }
      this->_impl->_socket->rdv_connect(id, host, port, timeout);
    }

    void
    UTPServer::set_local_id(std::string const& id)
    {
      this->_impl->_socket->set_local_id(id);
    }

    std::unique_ptr<RDVSocket> const &
    UTPServer::socket()
    {
      return this->_impl->socket();
    }

    /*-----.
    | Impl |
    `-----*/

    UTPServer::Impl::Impl()
      : _ctx(utp_init(2))
      , _xorify(0)
      , _accept_barrier("UTPServer accept")
      , _sending(false)
      , _icmp_fd(-1)
    {
      utp_context_set_userdata(this->_ctx, this);
      utp_set_callback(this->_ctx, UTP_ON_FIREWALL, &on_firewall);
      utp_set_callback(this->_ctx, UTP_ON_ACCEPT, &network::on_accept);
      utp_set_callback(this->_ctx, UTP_ON_ERROR, &on_error);
      utp_set_callback(this->_ctx, UTP_ON_STATE_CHANGE, &on_state_change);
      utp_set_callback(this->_ctx, UTP_ON_READ, &on_read);
      utp_set_callback(this->_ctx, UTP_ON_CONNECT, &on_connect);
      utp_set_callback(this->_ctx, UTP_SENDTO, &on_sendto);
      utp_set_callback(this->_ctx, UTP_LOG, &on_log);
      utp_context_set_option(this->_ctx, UTP_INITIAL_TIMEOUT, 300);
      utp_context_set_option(this->_ctx, UTP_TIMEOUT_INCRASE_PERCENT, 150);
      utp_context_set_option(this->_ctx, UTP_MAXIMUM_TIMEOUT, 5000);
    }

    UTPServer::Impl::~Impl()
    {
      try
      {
        ELLE_TRACE_SCOPE("%s: destroy", this);
        this->_cleanup();
      }
      catch (...)
      {
        ELLE_ERR("UTPServer exceptioned while shutting down: %s",
                 elle::exception_string());
        std::abort();
      }
    }

    void
    UTPServer::Impl::_check_icmp()
    {
#if defined(INFINIT_MACOSX)
      if (this->_icmp_fd == -1)
      {
        this->_icmp_fd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
        if (this->_icmp_fd < 0)
          elle::err("Failed to create ICMP socket: %s", errno);
        if (fcntl(this->_icmp_fd, F_SETFL, O_NONBLOCK) == -1)
          elle::err("Failed to set socket to non-blocking state: %s", errno);
      }
      unsigned char buf[4096];
      struct sockaddr_in sa;
      socklen_t sasz = sizeof(sa);
      int res =
        recvfrom(this->_icmp_fd, buf, 4096, 0, (struct sockaddr*)&sa, &sasz);
      if (res == -1 && (errno == EWOULDBLOCK || errno == EAGAIN))
        return;
      if (res == -1)
      {
        return;
      }
      ELLE_DUMP("%x", elle::Buffer(buf, res));
      // We received a full ip packet
      if (buf[9] != 1) // type ICMP
      {
        ELLE_DUMP("Not ICMP: %s", (unsigned int)buf[9]);
        return;
      }
      static const int offset_icmp = 20;
      static const int offset_payload_ip_dest = 44;
      static const int offset_payload_udp_dport = 50;
      static const int offset_udp_payload = 56;
      if (res < offset_udp_payload)
      {
        ELLE_DUMP("Payload too short (%s)", res);
        return;
      }
      sa.sin_addr.s_addr = *(uint32_t*)(buf + offset_payload_ip_dest);
      sa.sin_port = *(uint16_t*)(buf + offset_payload_udp_dport);
      int type = buf[offset_icmp];
      int code = buf[offset_icmp+1];
      ELLE_DEBUG("icmp type %s code %s ip %s port %s payload %s",
                 type, code, inet_ntoa(sa.sin_addr), ntohs(sa.sin_port),
                 res - offset_udp_payload);
      if (type == 3 && code == 4)
        utp_process_icmp_fragmentation(
          this->_ctx, buf + offset_udp_payload, res - offset_udp_payload,
          (struct sockaddr*)&sa,
          sizeof(sa), 0); // FIXME properly fill next_hop_mtu
      else
        utp_process_icmp_error(this->_ctx, buf+offset_udp_payload,
          res - offset_udp_payload,
          (struct sockaddr*)&sa,
          sizeof(sa));
#endif
      // Code comming straight from ucat libutp example.
#if defined(INFINIT_LINUX)
      int fd = this->_socket->socket()->native_handle();
      unsigned char vec_buf[4096], ancillary_buf[4096];
      struct iovec iov = { vec_buf, sizeof(vec_buf) };
      struct sockaddr_in remote;
      struct msghdr msg;
      ssize_t len;
      struct cmsghdr* cmsg;
      struct sock_extended_err* e;
      struct sockaddr* icmp_addr;
      struct sockaddr_in* icmp_sin;
      memset(&msg, 0, sizeof(msg));
      msg.msg_name = &remote;
      msg.msg_namelen = sizeof(remote);
      msg.msg_iov = &iov;
      msg.msg_iovlen = 1;
      msg.msg_flags = 0;
      msg.msg_control = ancillary_buf;
      msg.msg_controllen = sizeof(ancillary_buf);
      len = recvmsg(fd, &msg, MSG_ERRQUEUE);
      if (len < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
        ELLE_DEBUG("recvmsg error: %s", errno);
      if (len < 0)
        return;
      for (cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg))
      {
        ELLE_DEBUG("Handling one!");
        if (cmsg->cmsg_type != IP_RECVERR)
        {
          ELLE_DEBUG("Unhandled errqueue type: %s", cmsg->cmsg_type);
          continue;
        }
        if (cmsg->cmsg_level != SOL_IP)
        {
          ELLE_DEBUG("Unhandled errqueue level: %s", cmsg->cmsg_level);
          continue;
        }
        ELLE_DEBUG("errqueue: IP_RECVERR, SOL_IP, len %s", cmsg->cmsg_len);
        if (remote.sin_family != AF_INET)
        {
          ELLE_DEBUG("Address family is %s, not AF_INET? Ignoring",
                     remote.sin_family);
          continue;
        }
        ELLE_DEBUG("Remote host: %s:%s",
                   inet_ntoa(remote.sin_addr), ntohs(remote.sin_port));
        e = (struct sock_extended_err*)CMSG_DATA(cmsg);
        if (!e)
        {
          ELLE_DEBUG("errqueue: sock_extended_err is NULL?");
          continue;
        }
        if (e->ee_origin != SO_EE_ORIGIN_ICMP)
        {
          ELLE_DEBUG("errqueue: Unexpected origin: %d", e->ee_origin);
          continue;
        }
        ELLE_DEBUG("    ee_errno:  %s", e->ee_errno);
        ELLE_DEBUG("    ee_origin: %s", e->ee_origin);
        ELLE_DEBUG("    ee_type:   %s", e->ee_type);
        ELLE_DEBUG("    ee_code:   %s", e->ee_code);
        // discovered MTU for EMSGSIZE errors
        ELLE_DEBUG("    ee_info:   %s", e->ee_info);
        ELLE_DEBUG("    ee_data:   %s", e->ee_data);
        // "Node that caused the error"
        // "Node that generated the error"
        icmp_addr = (struct sockaddr*)SO_EE_OFFENDER(e);
        icmp_sin = (struct sockaddr_in*)icmp_addr;
        if (icmp_addr->sa_family != AF_INET)
        {
          ELLE_DEBUG("ICMP's address family is %s, not AF_INET?",
                     icmp_addr->sa_family);
          continue;
        }
        if (icmp_sin->sin_port != 0)
        {
          ELLE_DEBUG("ICMP's 'port' is not 0?");
          continue;
        }
        ELLE_DEBUG("msg_flags: %s", msg.msg_flags);
        if (false)
        {
          if (msg.msg_flags & MSG_TRUNC)    fprintf(stderr, " MSG_TRUNC");
          if (msg.msg_flags & MSG_CTRUNC)   fprintf(stderr, " MSG_CTRUNC");
          if (msg.msg_flags & MSG_EOR)      fprintf(stderr, " MSG_EOR");
          if (msg.msg_flags & MSG_OOB)      fprintf(stderr, " MSG_OOB");
          if (msg.msg_flags & MSG_ERRQUEUE) fprintf(stderr, " MSG_ERRQUEUE");
          fprintf(stderr, "\n");
        }
        if (e->ee_type == 3 && e->ee_code == 4)
        {
          ELLE_TRACE(
            "ICMP type 3, code 4: Fragmentation error, discovered MTU %s",
            e->ee_info);
          utp_process_icmp_fragmentation(this->_ctx, vec_buf, len,
                                         (struct sockaddr*)&remote,
                                         sizeof(remote), e->ee_info);
        }
        else
        {
          ELLE_TRACE("ICMP type %s, code %s", e->ee_type, e->ee_code);
          utp_process_icmp_error(this->_ctx, vec_buf, len,
                                 (struct sockaddr *)&remote, sizeof(remote));
        }
      }
#endif
    }

    void
    UTPServer::Impl::listen(EndPoint const& ep)
    {
      this->_socket = elle::make_unique<RDVSocket>();
      this->_socket->close();
      this->_socket->bind(ep);
#ifdef INFINIT_LINUX
      int on = 1;
      /* Set the option, so we can receive errors */
      setsockopt(this->_socket->socket()->native_handle(), SOL_IP, IP_RECVERR,
                 (char*)&on, sizeof(on));
#endif
      this->_listener = elle::make_unique<Thread>(
        elle::sprintf("UTPServer(%s)", this->_socket->local_endpoint().port()),
        [this]
        {
          elle::Buffer buf;
          while (true)
          {
            buf.size(20000);
            EndPoint source;
            int sz = 0;
            try
            {
              if (!this->_socket->socket()->is_open())
              {
                ELLE_DEBUG("Socket closed, exiting");
                return;
              }
              sz = this->_socket->receive_from
                (elle::WeakBuffer(buf.mutable_contents(), buf.size()), source);
              buf.size(sz);
              if (this->_xorify)
              {
                for (int i= 0; i < sz; ++i)
                  buf[i] ^= this->_xorify;
              }
              auto* raw = source.data();
              ELLE_TRACE("%s: received %s bytes", this, sz);
              utp_process_udp(this->_ctx, buf.contents(), sz, raw, source.size());
              utp_issue_deferred_acks(this->_ctx);
            }
            catch (reactor::Terminate const&)
            {
              this->_cleanup();
              throw;
            }
            catch (std::exception const& e)
            {
              ELLE_TRACE("listener exception %s", e.what());
              // go on, this error might concern one of the many peers we deal
              // with.
            }
          }
        });
      this->_checker.reset(new Thread("checker", [this] {
            try
            {
              while (true)
              {
                auto& sst = this->_socket_shutdown_threads;
                auto it = std::remove_if(sst.begin(), sst.end(),
                  [](Thread::unique_ptr const& t)
                  {
                    return !t || t->done();
                  });
                sst.erase(it, sst.end());

                utp_check_timeouts(this->_ctx);
                reactor::sleep(50_ms);
                this->_check_icmp();
              }
            }
            catch (...)
            {
              ELLE_DEBUG("%s: exiting checker: %s",
                         this, elle::exception_string());
              throw;
            }
          }));
      ELLE_TRACE("%s: listening on %s", this, this->_socket->local_endpoint());
    }

    void
    UTPServer::Impl::send_to(Buffer buf, EndPoint where)
    {
      this->_send_buffer.emplace_back(elle::Buffer(buf.data(), buf.size()),
                                      where);
      if (!this->_sending)
      {
        this->_sending = true;
        this->_send();
      }
      else
        ELLE_DEBUG("already sending, data queued");
    }

    void
    UTPServer::Impl::on_accept(utp_socket* s)
    {
      this->_accept_queue.emplace_back(
        new UTPSocket(elle::make_unique<UTPSocket::Impl>(
                        this->shared_from_this(), s, true)));
      this->_accept_barrier.open();
    }

    std::unique_ptr<UTPSocket>
    UTPServer::Impl::accept()
    {
      ELLE_TRACE_SCOPE("%s: accept", this);
      this->_accept_barrier.wait();
      ELLE_ASSERT(this->_accept_barrier.opened());
      std::unique_ptr<UTPSocket> sock(this->_accept_queue.back().release());
      this->_accept_queue.pop_back();
      if (this->_accept_queue.empty())
        this->_accept_barrier.close();
      ELLE_TRACE("%s: accepted %s", this, sock);
      return sock;
    }

    void
    UTPServer::Impl::_send()
    {
      auto& buf = this->_send_buffer.front();
      ELLE_TRACE_SCOPE(
        "%s: send %s UDP bytes to %s", this, buf.first.size(), buf.second);
      // At least on windows, passing a v4 address to send_to() on a v6 socket
      // is an error
      auto endpoint = buf.second;
      if (endpoint.address().is_v4() &&
          this->_socket->local_endpoint().address().is_v6())
        endpoint = EndPoint(boost::asio::ip::address_v6::v4_mapped(
                              endpoint.address().to_v4()), endpoint.port());
      this->_socket->socket()->async_send_to(
        boost::asio::buffer(buf.first.contents(), buf.first.size()),
        endpoint,
        [this] (boost::system::error_code const& errc, size_t size)
        { this->_send_cont(errc, size); });
    };

    void
    UTPServer::Impl::_send_cont(boost::system::error_code const& erc, size_t)
    {
      if (erc == boost::asio::error::operation_aborted)
        return;
      if (erc)
        ELLE_WARN("%s: send_to error: %s", this, erc.message());
      this->_send_buffer.pop_front();
      if (this->_send_buffer.empty())
        this->_sending = false;
      else
        this->_send();
    }

    void
    UTPServer::Impl::_cleanup()
    {
      ELLE_TRACE_SCOPE("%s: cleanup", *this);
      // Run any completed callback before deleting this.
      {
        reactor::scheduler().io_service().reset();
        reactor::scheduler().io_service().poll();
      }
      if (this->_socket)
      { // Was never initialized.
        if (this->_checker)
        {
          this->_checker->terminate();
          reactor::wait(*this->_checker);
        }
        if (this->_listener)
        {
          this->_listener->terminate();
          reactor::wait(*this->_listener);
        }
        this->_socket->socket()->close();
        this->_socket->close();
        this->_socket.reset(nullptr);
      }
      utp_destroy(this->_ctx);
    }
  }
}
