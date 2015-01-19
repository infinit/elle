
#define FUSE_USE_VERSION 26
#define _FILE_OFFSET_BITS 64

#include <sys/mount.h>

#include "fuse.hh"

#include <fuse/fuse.h>
#include <fuse/fuse_lowlevel.h>

#include <boost/asio.hpp>

#include <elle/Buffer.hh>
#include <elle/log.hh>
#include <elle/finally.hh>
#include <reactor/Barrier.hh>
#include <reactor/semaphore.hh>
#include <reactor/scheduler.hh>
#include <reactor/MultiLockBarrier.hh>
#include <reactor/exception.hh>

ELLE_LOG_COMPONENT("reactor.filesystem.fuse");

namespace reactor
{
  void FuseContext::loop()
  {
    fuse_session* s = fuse_get_session(_fuse);
    fuse_chan* ch = fuse_session_next_chan(s, NULL);
    int fd = fuse_chan_fd(ch);
    ELLE_TRACE("Got fuse fs %s", fd);
    boost::asio::posix::stream_descriptor socket(scheduler().io_service());
    socket.assign(fd);
    auto lock = _mt_barrier.lock();
    while (!fuse_exited(_fuse))
    {
      _socket_barrier.close();
      socket.async_read_some(boost::asio::null_buffers(),
        [&](boost::system::error_code const&, std::size_t)
        {
          _socket_barrier.open();
        });
      ELLE_DUMP("waiting for socket");
      wait(_socket_barrier);
      if (fuse_exited(_fuse))
        break;
      ELLE_DUMP("Processing command");
      //highlevel api
      fuse_cmd* cmd = fuse_read_cmd(_fuse);
      if (!cmd)
        break;
      fuse_process_cmd(_fuse, cmd);
    }
    socket.release();
  }

  void FuseContext::loop_pool(int nThreads)
  {
    Scheduler& sched = scheduler();
#ifdef INFINIT_MACOSX
    _loopThread.reset(new std::thread([=](Scheduler* sched) { this->_loop_pool(nThreads, *sched);}, &sched));
#else
    _loop.reset(new Thread("fuse loop",
                           [=] { this->_loop_pool(nThreads, scheduler());}));
#endif
  }

  void FuseContext::loop_mt()
  {
    Scheduler& sched = scheduler();
#ifdef INFINIT_MACOSX
    _loopThread.reset(new std::thread([&] { this->_loop_mt(sched);}));
#else
    _loop.reset(new Thread("fuse loop",
                           [&] { this->_loop_mt(sched);}));
#endif
  }

  void FuseContext::_loop_pool(int nThreads, Scheduler& sched)
  {
    ELLE_TRACE("Entering pool loop with %s workers", nThreads);
    reactor::Semaphore sem;
    bool stop = false;
    std::list<elle::Buffer> requests;
    fuse_session* s = fuse_get_session(_fuse);
    fuse_chan* ch = fuse_session_next_chan(s, NULL);
    size_t buffer_size = fuse_chan_bufsize(ch);
    auto lock = _mt_barrier.lock();
    auto worker = [&] {
      auto lock = _mt_barrier.lock();
      while (true)
      {
        sem.wait();
        elle::Buffer buf;
        {
#ifdef INFINIT_MACOSX
          std::unique_lock<std::mutex> mutex_lock(_mutex);
#endif
          if (stop)
            return;
          if (requests.empty())
          {
            ELLE_WARN("Worker woken up with empty queue");
            continue;
          }
          buf = std::move(requests.front());
          requests.pop_front();
        }
        ELLE_TRACE("Processing new request");
        fuse_session_process(s, (const char*)buf.mutable_contents(), buf.size(), ch);
        ELLE_TRACE("Back to the pool");
      }
    };
    for(int i=0; i< nThreads; ++i)
    {
      Thread* t = new Thread(elle::sprintf("fuse worker %s", i), worker);
      _workers.push_back(t);
    }
#ifndef INFINIT_MACOSX
    int fd = fuse_chan_fd(ch);
    ELLE_TRACE("Got fuse fs %s", fd);
    boost::asio::posix::stream_descriptor socket(scheduler().io_service());
    socket.assign(fd);
#endif
    while (!fuse_exited(_fuse))
    {
#ifndef INFINIT_MACOSX
      _socket_barrier.close();
      socket.async_read_some(boost::asio::null_buffers(),
        [&](boost::system::error_code const& erc, std::size_t)
        {
          if (erc)
            return;
          ELLE_DUMP("fuse message ready, opening...");
          _socket_barrier.open();
        });
      ELLE_DUMP("waiting for socket");
      wait(_socket_barrier);
#endif
      if (fuse_exited(_fuse))
        break;
      ELLE_DUMP("Processing command");
      elle::Buffer buf;
      buf.size(buffer_size);
      int res = -EINTR;
      while(res == -EINTR)
        res = fuse_chan_recv(&ch, (char*)buf.mutable_contents(), buf.size());
      if (res == -EAGAIN)
        continue;
      if (res <= 0)
      {
        ELLE_LOG("%s: %s", res, strerror(-res));
        break;
      }
      buf.size(res);
#ifdef INFINIT_MACOSX
      std::unique_lock<std::mutex> mutex_lock(_mutex);
#endif
      requests.push_back(std::move(buf));
      sem.release();
    }
    stop = true;
    for (int i = 0; i < signed(this->_workers.size()); ++i)
      sem.release();
    for(auto t : _workers)
      reactor::wait(*t);
  }


  void FuseContext::_loop_mt(Scheduler& sched)
  {
    reactor::MultiLockBarrier barrier;
    fuse_session* s = fuse_get_session(_fuse);
    fuse_chan* ch = fuse_session_next_chan(s, NULL);
    size_t buffer_size = fuse_chan_bufsize(ch);
#ifndef INFINIT_MACOSX
    int fd = fuse_chan_fd(ch);
    ELLE_TRACE("Got fuse fs %s", fd);
    boost::asio::posix::stream_descriptor socket(scheduler().io_service());
    socket.assign(fd);
#endif
    auto lock = _mt_barrier.lock();
    void* buffer_data = malloc(buffer_size);
    while (!fuse_exited(_fuse))
    {
#ifndef INFINIT_MACOSX
      _socket_barrier.close();
      socket.async_read_some(boost::asio::null_buffers(),
        [&](boost::system::error_code const& erc, std::size_t)
        {
          if (erc)
            return;
          _socket_barrier.open();
        });
      ELLE_DUMP("waiting for socket");
      wait(_socket_barrier);
#endif
      if (fuse_exited(_fuse))
        break;
      ELLE_DUMP("Processing command");
      int res = -EINTR;
      while(res == -EINTR)
        res = fuse_chan_recv(&ch, (char*)buffer_data, buffer_size);
      if (res == -EAGAIN)
        continue;
      if (res <= 0)
      {
        ELLE_LOG("%s: %s", res, strerror(-res));
        break;
      }
      void* b2 = malloc(res);
      memcpy(b2, buffer_data, res);
#ifdef INFINIT_MACOSX
      std::unique_lock<std::mutex> mutex_lock(_mutex);
#endif
      _workers.push_back(new Thread(sched, "fuse worker", [s, b2, res, ch, this] {
        auto lock = this->_mt_barrier.lock();
        fuse_session_process(s, (const char*)b2, res, ch);
        free(b2);
#ifdef INFINIT_MACOSX
        std::unique_lock<std::mutex> mutex_lock(_mutex);
#endif
        auto it = std::find(_workers.begin(), _workers.end(), scheduler().current());
        ELLE_ASSERT(it != _workers.end());
        std::swap(*it, _workers.back());
        _workers.pop_back();
        },
        true));
    }
  }

  void FuseContext::create(std::string const& mountpoint,
                    std::vector<std::string> const& arguments,
                    const struct fuse_operations *op, size_t op_size,
                    void* user_data)
  {
    _mountpoint = mountpoint;
    fuse_args args;
    args.allocated = false;
    args.argc = arguments.size();
    args.argv = (char**) malloc(sizeof(void*) * (arguments.size() + 1));
    void* ptr = args.argv;
    elle::SafeFinally cleanup([&] { if (ptr == args.argv) free(args.argv);});
    for (unsigned int i=0; i<arguments.size(); ++i)
      args.argv[i] = (char*)arguments[i].c_str();
    args.argv[arguments.size()] = nullptr;
    fuse_chan* chan = ::fuse_mount(mountpoint.c_str(), &args);
    if (!chan)
      throw std::runtime_error("fuse_mount failed");
    _fuse = ::fuse_new(chan, &args, op, op_size, user_data);
    if (!_fuse)
      throw std::runtime_error("fuse_new failed");
  }

  void FuseContext::destroy(DurationOpt graceTime)
  {
    ELLE_TRACE("fuse_destroy");
    ::fuse_exit(_fuse);
    _socket_barrier.open();
    ELLE_TRACE("terminating...");
    try
    {
      reactor::wait(_mt_barrier, graceTime);
    }
    catch(Timeout const&)
    {
      ELLE_TRACE("killing...");
      if (_loop)
        _loop->terminate_now();
      for (auto const& t: _workers)
        t->terminate_now();
      reactor::wait(_mt_barrier);
    }
    if (_loopThread)
      _loopThread->join();
    ELLE_TRACE("done");
    fuse_session* s = ::fuse_get_session(_fuse);
    ELLE_TRACE("session");
    fuse_chan* ch = ::fuse_session_next_chan(s, NULL);
    ELLE_TRACE("chan %s", ch);
#ifdef INFINIT_MACOSX
    int fd = fuse_chan_fd(ch);
    if (fd > 0)
      ::close(fd);
    ::unmount(_mountpoint.c_str(), MNT_FORCE); // MNT_FORCE
#else
    ::fuse_unmount(_mountpoint.c_str(), ch);
#endif
    ELLE_TRACE("unomount");
    ::fuse_destroy(_fuse);
    ELLE_TRACE("destroy");
  }
}
