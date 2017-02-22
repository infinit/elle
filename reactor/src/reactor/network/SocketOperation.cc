#include <boost/asio/ssl.hpp>

#include <elle/log.hh>

#include <reactor/network/SocketOperation.hh>
#include <reactor/scheduler.hh>

ELLE_LOG_COMPONENT("reactor.network.SocketOperation");

namespace reactor
{
  namespace network
  {
    /*----------------.
    | SocketOperation |
    `----------------*/

    template <typename AsioSocket>
    SocketOperation<AsioSocket>::SocketOperation(
      AsioSocket& socket):
      Operation(*reactor::Scheduler::scheduler()),
      _socket(socket),
      _canceled(false)
    {}

    template <typename AsioSocket>
    void
    SocketOperation<AsioSocket>::_abort()
    {
      ELLE_TRACE_SCOPE("%s: abort", *this);
      this->_canceled = true;
      boost::system::error_code ec;
      this->_socket.cancel(ec);
      // Cancel may fail if for instance the socket was closed manually. If
      // cancel fails, assume the operation is de facto cancelled and we can
      // carry on. I know of no case were we "were not actually able to
      // cancel the operation".
      (void) ec;
      reactor::wait(*this);
    }

    template <typename AsioSocket>
    void
    SocketOperation<AsioSocket>::_wakeup(
      const boost::system::error_code& error)
    {
      if (this->_canceled)
        ELLE_TRACE_SCOPE("%s: ended: cancelled", *this);
      else if (error)
      {
        ELLE_TRACE_SCOPE("%s: ended with error: %s (%s)", *this, error.message(), error);
        this->_handle_error(error);
      }
      else
        ELLE_TRACE_SCOPE("%s: ended", *this);
      this->done();
    }

    template <typename AsioSocket>
    void
    SocketOperation<AsioSocket>::_handle_error(
      const boost::system::error_code& error)
    {
      if (error == boost::asio::error::connection_refused
          || error.value() == EADDRNOTAVAIL)
        this->_raise<ConnectionRefused>();
      else
        this->_raise<Exception>(error.message());
    }

    template class SocketOperation<boost::asio::ip::tcp::socket>;
    template class SocketOperation<boost::asio::ip::udp::socket>;
#ifdef REACTOR_NETWORK_UNIX_DOMAIN_SOCKET
    template class SocketOperation<boost::asio::local::stream_protocol::socket>;
#endif

    /*--------------.
    | DataOperation |
    `--------------*/

    template <typename AsioSocket>
    DataOperation<AsioSocket>::DataOperation(AsioSocket& socket)
      : Super(socket)
    {}

    template <typename AsioSocket>
    void
    DataOperation<AsioSocket>::_handle_error(
      boost::system::error_code const& error)
    {
      if (error == boost::asio::error::eof ||
          error == boost::asio::error::broken_pipe ||
          error == boost::asio::error::connection_aborted ||
          error == boost::asio::error::connection_reset ||
          error == boost::asio::error::operation_aborted)
        this->template _raise<ConnectionClosed>();
      else if (error == boost::asio::error::bad_descriptor)
        this->template _raise<SocketClosed>();
      else if (error.category() == boost::asio::error::get_ssl_category() &&
               error.value() == ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SHORT_READ))
        this->template _raise<SSLShortRead>();
      else
        Super::_handle_error(error);
    }

    template class DataOperation<boost::asio::ip::tcp::socket>;
    template class DataOperation<boost::asio::ip::udp::socket>;
#ifdef REACTOR_NETWORK_UNIX_DOMAIN_SOCKET
    template class DataOperation<boost::asio::local::stream_protocol::socket>;
#endif
  }
}

#ifdef INFINIT_LINUX

namespace reactor
{
  namespace network
  {
    static
    std::unordered_map<reactor::Thread*, std::function<void()>>
    _epoll_interrupt_callback;

    void
    epoll_interrupt_callback(std::function<void()> cb,
                                     reactor::Thread* thread)
    {
      if (!cb)
        _epoll_interrupt_callback.erase(thread);
      else
        _epoll_interrupt_callback[thread] = cb;
    }
  }
}

extern "C"
int
reactor_epoll_wait(int epfd, struct epoll_event *events,
                  int maxevents, int timeout)
{
  ELLE_DEBUG("epoll_wait %s %s", epfd, timeout);
  int res = epoll_wait(epfd, events, maxevents, 0);
  if (res || !timeout)
  {
    ELLE_DUMP("early return %s", res);
    return res;
  }
  auto& ios = reactor::scheduler().io_service();
  boost::asio::posix::stream_descriptor s(ios);
  s.assign(epfd);
  boost::asio::deadline_timer timer(ios);
  reactor::Barrier b;
  bool timer_running = true;
  bool socket_running = true;
  if (timeout < 0)
    timer_running = false;
  else
  {
    timer.expires_from_now(boost::posix_time::milliseconds(timeout));
    timer.async_wait([&](boost::system::error_code erc) {
        ELLE_DUMP("timer_exp %s", erc);
        timer_running = false;
        if (!erc && socket_running)
          s.cancel();
        if (!socket_running)
          b.open();
    });
  }
  int rv = 0;
  s.async_read_some(boost::asio::null_buffers(),
    [&] (boost::system::error_code erc, std::size_t bt) {
      ELLE_DUMP("async_read_some %s %s", erc, bt);
      socket_running = false;
      s.release();
      if (!erc)
        rv = 1;
      if (timer_running)
        timer.cancel();
      else
        b.open();
  });
  try
  {
    reactor::wait(b);
  }
  catch (...)
  {
    ELLE_TRACE("epoll wrapper interrupted by %s", elle::exception_string());
    try
    {
      if (socket_running)
        s.cancel();
      elle::With<reactor::Thread::NonInterruptible>()
        << [&](reactor::Thread::NonInterruptible&) {
        reactor::wait(b);
      };
    }
    catch (...)
    {
      ELLE_ERR("reactor_epoll_wait threw again: %s", elle::exception_string());
    }
    auto it = reactor::network::_epoll_interrupt_callback.find(
      reactor::scheduler().current());
    if (it != reactor::network::_epoll_interrupt_callback.end())
    {
      it->second();
      errno = EINTR;
      return -1;
    }
    else
      throw;
  }
  ELLE_DUMP("returned: %s", rv);
  if (rv)
    return epoll_wait(epfd, events, maxevents, 0);
  else
    return 0;
}
extern "C"
int
reactor_epoll_pwait(int epfd, struct epoll_event *events,
                   int maxevents, int timeout,
                   const sigset_t *sigmask)
{
  return reactor_epoll_wait(epfd, events, maxevents, timeout);
}

#endif
