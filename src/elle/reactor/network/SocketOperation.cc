#include <boost/asio/ssl.hpp>

#include <elle/log.hh>
#include <elle/reactor/network/SocketOperation.hh>
#include <elle/reactor/scheduler.hh>

#include <elle/reactor/network/SocketOperation.hxx>

ELLE_LOG_COMPONENT("elle.reactor.network.SocketOperation");

#ifdef INFINIT_LINUX
namespace
{
  auto _epoll_interrupt_callback
  = std::unordered_map<elle::reactor::Thread*, std::function<void()>>{};
}

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      void
      epoll_interrupt_callback(std::function<void()> cb,
                               elle::reactor::Thread* thread)
      {
        if (cb)
          _epoll_interrupt_callback[thread] = cb;
        else
          _epoll_interrupt_callback.erase(thread);
      }
    }
  }
}

extern "C"
int
reactor_epoll_wait(int epfd, struct epoll_event *events,
                  int maxevents, int timeout)
{
  if (!elle::reactor::Scheduler::scheduler())
    return epoll_wait(epfd, events, maxevents, timeout);
  ELLE_DEBUG("epoll_wait %s %s", epfd, timeout);
  int res = epoll_wait(epfd, events, maxevents, 0);
  if (res || !timeout)
  {
    ELLE_DUMP("early return %s", res);
    return res;
  }
  auto& ios = elle::reactor::scheduler().io_service();
  boost::asio::posix::stream_descriptor s(ios);
  s.assign(epfd);
  boost::asio::deadline_timer timer(ios);
  elle::reactor::Barrier b;
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
    elle::reactor::wait(b);
  }
  catch (...)
  {
    ELLE_TRACE("epoll wrapper interrupted by %s", elle::exception_string());
    try
    {
      if (socket_running)
        s.cancel();
      elle::With<elle::reactor::Thread::NonInterruptible>()
        << [&](elle::reactor::Thread::NonInterruptible&) {
        elle::reactor::wait(b);
      };
    }
    catch (...)
    {
      ELLE_ERR("reactor_epoll_wait threw again: %s", elle::exception_string());
    }
    auto it = _epoll_interrupt_callback.find(
      elle::reactor::scheduler().current());
    if (it != _epoll_interrupt_callback.end())
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
