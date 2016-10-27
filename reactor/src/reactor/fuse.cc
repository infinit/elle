
#define FUSE_USE_VERSION 26
#define _FILE_OFFSET_BITS 64

#include <sys/mount.h>

#include <reactor/fuse.hh>
#ifdef INFINIT_MACOSX
# include <CoreFoundation/CoreFoundation.h>
# include <DiskArbitration/DiskArbitration.h>
#endif

#include <fuse/fuse.h>
#include <fuse/fuse_lowlevel.h>

#include <boost/asio.hpp>

#include <elle/Buffer.hh>
#include <elle/log.hh>
#include <elle/system/Process.hh>
#include <elle/finally.hh>
#include <reactor/Barrier.hh>
#include <reactor/semaphore.hh>
#include <reactor/scheduler.hh>
#include <reactor/MultiLockBarrier.hh>
#include <reactor/exception.hh>

ELLE_LOG_COMPONENT("reactor.filesystem.fuse");

namespace reactor
{
  FuseContext::FuseContext()
    : _mt_barrier(elle::sprintf("%s barrier", this))
  {}

  void
  FuseContext::loop()
  {
    // Macos can't run async ops on fuse socket, so thread it
#ifdef INFINIT_MACOSX
    this->_loop.reset(new Thread(
      "holder",
      [&]
      {
        reactor::sleep();
      }));
    auto& sched = scheduler();
    this->_loop_thread.reset(new std::thread(
      [&]
      {
        this->_loop_one_thread(sched);
      }));
#else
    this->_loop.reset(new Thread("fuse loop",
      [&]
      {
        this->_loop_single();
      }));
#endif
  }

  void
  FuseContext::_loop_one_thread(reactor::Scheduler& sched)
  {
    fuse_session* s = fuse_get_session(this->_fuse);
    fuse_chan* ch = fuse_session_next_chan(s, NULL);
    size_t buffer_size = fuse_chan_bufsize(ch);
    void* buffer_data = malloc(buffer_size);
    while (!fuse_exited(this->_fuse))
    {
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
      try
      {
        sched.mt_run<void>(
          "fuse worker",
          [&]
          {
            fuse_session_process(s, (const char*)buffer_data, res, ch);
          });
      }
      catch (std::exception const& e)
      {
        ELLE_WARN("Exception escaped fuse_process: %s", e.what());
      }
    }
    if (this->on_loop_exited())
      sched.run_later("exit notifier", this->on_loop_exited());
  }

  void
  FuseContext::_loop_single()
  {
    fuse_session* s = fuse_get_session(this->_fuse);
    fuse_chan* ch = fuse_session_next_chan(s, NULL);
    int fd = fuse_chan_fd(ch);
    ELLE_TRACE("Got fuse fs %s", fd);
    boost::asio::posix::stream_descriptor socket(scheduler().io_service());
    socket.assign(fd);
    auto lock = this->_mt_barrier.lock();
    while (!fuse_exited(this->_fuse))
    {
      this->_socket_barrier.close();
      socket.async_read_some(boost::asio::null_buffers(),
        [&] (boost::system::error_code const&, std::size_t)
        {
          if (!this->_fuse)
            return;
          this->_socket_barrier.open();
        });
      ELLE_DUMP("waiting for socket");
      wait(this->_socket_barrier);
      if (fuse_exited(this->_fuse))
        break;
      ELLE_DUMP("Processing command");
      //highlevel api
      fuse_cmd* cmd = fuse_read_cmd(this->_fuse);
      if (!cmd)
        break;
      fuse_process_cmd(this->_fuse, cmd);
    }
    socket.release();
    if (this->on_loop_exited())
      new reactor::Thread("exit notifier", this->on_loop_exited(), true);
  }

  void
  FuseContext::loop_pool(int threads)
  {
#ifdef INFINIT_MACOSX
    Scheduler& sched = scheduler();
    this->_loop_thread.reset(new std::thread(
      [=] (Scheduler* sched)
      {
        this->_loop_pool(threads, *sched);
      }, &sched));
#else
    this->_loop.reset(new Thread(
      "fuse loop",
      [=]
      {
        this->_loop_pool(threads, scheduler());
      }));
#endif
  }

  void
  FuseContext::loop_mt()
  {
    Scheduler& sched = scheduler();
#ifdef INFINIT_MACOSX
    this->_loop_thread.reset(new std::thread(
      [&]
      {
        this->_loop_mt(sched);
      }));
#else
    this->_loop.reset(new Thread(
      "fuse loop",
      [&]
      {
        this->_loop_mt(sched);
      }));
#endif
  }

  void
  FuseContext::_loop_pool(int threads, Scheduler& sched)
  {
    ELLE_TRACE("Entering pool loop with %s workers", threads);
    reactor::Semaphore sem;
    bool stop = false;
    std::list<elle::Buffer> requests;
    fuse_session* s = fuse_get_session(this->_fuse);
    fuse_chan* ch = fuse_session_next_chan(s, NULL);
    size_t buffer_size = fuse_chan_bufsize(ch);
    auto lock = this->_mt_barrier.lock();
    auto worker = [&] {
      auto lock = this->_mt_barrier.lock();
      while (true)
      {
        sem.wait();
        elle::Buffer buf;
        {
#ifdef INFINIT_MACOSX
          std::unique_lock<std::mutex> mutex_lock(this->_mutex);
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
        fuse_session_process(
          s, (const char*)buf.mutable_contents(), buf.size(), ch);
        ELLE_TRACE("Back to the pool");
      }
    };
    for(int i = 0; i < threads; ++i)
    {
      Thread* t = new Thread(elle::sprintf("fuse worker %s", i), worker);
      this->_workers.push_back(t);
    }
#ifndef INFINIT_MACOSX
    int fd = fuse_chan_fd(ch);
    ELLE_TRACE("Got fuse fs %s", fd);
    boost::asio::posix::stream_descriptor socket(scheduler().io_service());
    socket.assign(fd);
#endif
    while (!fuse_exited(this->_fuse))
    {
#ifndef INFINIT_MACOSX
      this->_socket_barrier.close();
      socket.async_read_some(boost::asio::null_buffers(),
        [&] (boost::system::error_code const& erc, std::size_t)
        {
          if (erc)
            return;
          ELLE_DUMP("fuse message ready, opening...");
          this->_socket_barrier.open();
        });
      ELLE_DUMP("waiting for socket");
      wait(this->_socket_barrier);
#endif
      if (fuse_exited(this->_fuse))
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
      std::unique_lock<std::mutex> mutex_lock(this->_mutex);
#endif
      requests.push_back(std::move(buf));
      sem.release();
    }
    ELLE_DEBUG("Exiting worker threads");
    stop = true;
    for (int i = 0; i < signed(this->_workers.size()); ++i)
      sem.release();
    for(auto t : _workers)
      reactor::wait(*t);
    ELLE_DEBUG("fuse loop returning");
    if (this->on_loop_exited())
#ifdef INFINIT_MACOSX
      sched.mt_run<void>("exit notifier", this->on_loop_exited());
#else
      new reactor::Thread("exit notifier", this->on_loop_exited(), true);
#endif
  }


  void
  FuseContext::_loop_mt(Scheduler& sched)
  {
    fuse_session* s = fuse_get_session(this->_fuse);
    fuse_chan* ch = fuse_session_next_chan(s, NULL);
    size_t buffer_size = fuse_chan_bufsize(ch);
#ifndef INFINIT_MACOSX
    int fd = fuse_chan_fd(ch);
    ELLE_TRACE("Got fuse fd %s", fd);
    boost::asio::posix::stream_descriptor socket(scheduler().io_service());
    socket.assign(fd);
#endif
    auto lock = this->_mt_barrier.lock();
    void* buffer_data = malloc(buffer_size);
    while (!fuse_exited(this->_fuse))
    {
#ifndef INFINIT_MACOSX
      this->_socket_barrier.close();
      socket.async_read_some(boost::asio::null_buffers(),
        [&](boost::system::error_code const& erc, std::size_t)
        {
          if (erc)
            return;
          this->_socket_barrier.open();
        });
      ELLE_DUMP("waiting for socket");
      wait(this->_socket_barrier);
#endif
      if (fuse_exited(this->_fuse))
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
      std::unique_lock<std::mutex> mutex_lock(this->_mutex);
#endif
      this->_workers.push_back(new Thread(
        sched,
        "fuse worker",
        [s, b2, res, ch, this]
        {
          auto lock = this->_mt_barrier.lock();
          fuse_session_process(s, (const char*)b2, res, ch);
          free(b2);
#ifdef INFINIT_MACOSX
          std::unique_lock<std::mutex> mutex_lock(this->_mutex);
#endif
          auto it = std::find(this->_workers.begin(),
                              this->_workers.end(),
                              scheduler().current());
          ELLE_ASSERT(it != this->_workers.end());
          std::swap(*it, this->_workers.back());
          this->_workers.pop_back();
        }, true));
    }
    if (this->on_loop_exited())
#ifdef INFINIT_MACOSX
      sched.mt_run<void>("exit notifier", this->on_loop_exited());
#else
      new reactor::Thread("exit notifier", this->on_loop_exited(), true);
#endif
  }

  void
  FuseContext::create(std::string const& mountpoint,
                      std::vector<std::string> const& arguments,
                      const struct fuse_operations* op,
                      size_t op_size,
                      void* user_data)
  {
    this->_mountpoint = mountpoint;
    fuse_args args;
    args.allocated = false;
    args.argc = arguments.size();
    args.argv = (char**) malloc(sizeof(void*) * (arguments.size() + 1));
    void* ptr = args.argv;
    elle::SafeFinally cleanup([&] { if (ptr == args.argv) free(args.argv);});
    for (unsigned int i = 0; i < arguments.size(); ++i)
      args.argv[i] = (char*)arguments[i].c_str();
    args.argv[arguments.size()] = nullptr;
    fuse_chan* chan = ::fuse_mount(mountpoint.c_str(), &args);
    if (!chan)
      throw std::runtime_error("fuse_mount failed");
    this->_fuse = ::fuse_new(chan, &args, op, op_size, user_data);
    if (!this->_fuse)
      throw std::runtime_error("fuse_new failed");
  }

#ifdef INFINIT_MACOSX
  static
  void
  _signal_handler(int sig)
  {
    ELLE_DEBUG("caught signal: %d", sig);
  }
#endif

  void
  FuseContext::destroy(DurationOpt grace_time)
  {
    ELLE_TRACE("fuse_destroy");
    if (this->_fuse)
    {
      ::fuse_exit(this->_fuse);
    }
    else
    {
      ELLE_TRACE("Already destroyed");
      return;
    }
    this->_socket_barrier.open();
    ELLE_TRACE("terminating...");
#ifndef INFINIT_MACOSX
    try
    {
      reactor::wait(this->_mt_barrier, grace_time);
    }
    catch (Timeout const&)
    {
      this->kill();
      reactor::wait(this->_mt_barrier);
    }
#endif
    if (this->_loop_thread)
    {
#ifdef INFINIT_MACOSX
      // Use a signal to stop the read syscall in fuse_chan_recv. We also need
      // to ensure that the syscall is not automatically restarted (default on
      // OS X, see `man signal`).
      struct sigaction action;
      sigaction(SIGUSR1, NULL, &action);
      action.sa_handler = &_signal_handler;
      action.sa_flags &= ~SA_RESTART;
      sigaction(SIGUSR1, &action, NULL);
      int res = 0;
      sigset_t mask_set;
      sigemptyset(&mask_set);
      sigaddset(&mask_set, SIGUSR1);
      res = pthread_sigmask(SIG_BLOCK, &mask_set, NULL);
      if (res != 0)
        ELLE_WARN("failed to mask SIGUSR1 on main thread, error: %d", res);
      res = pthread_kill(this->_loop_thread->native_handle(), SIGUSR1);
      if (res != 0)
        ELLE_WARN("failed to send signal to loop_thread, error: %d", res);
#endif
      this->_loop_thread->join();
    }
    ELLE_TRACE("done");
    if (!this->_fuse)
      return;
#ifndef INFINIT_MACOSX
    fuse_session* s = ::fuse_get_session(this->_fuse);
    ELLE_TRACE("session");
    fuse_chan* ch = ::fuse_session_next_chan(s, NULL);
    ELLE_TRACE("chan %s", (void*)(ch));
    ::fuse_unmount(this->_mountpoint.c_str(), ch);
#endif
    ELLE_TRACE("unmounted");
    ::fuse_destroy(this->_fuse);
    this->_fuse = nullptr;
    ELLE_TRACE("destroyed");
#ifdef INFINIT_MACOSX
    this->_loop->terminate_now();
    this->_mac_unmount(grace_time);
#endif
    ELLE_TRACE("finished");
  }

  void
  FuseContext::kill()
  {
#ifndef INFINIT_MACOSX
    if (this->_loop)
      this->_loop->terminate_now();
#endif
  }

#ifdef INFINIT_MACOSX
  static
  void
  _unmount_callback(DADiskRef disk, DADissenterRef dissenter, void* context)
  {
    if (dissenter)
    {
      DAReturn res = DADissenterGetStatus(dissenter);
      if (res != kDAReturnSuccess)
        ELLE_ERR("error unmounting (0x%x), see DADissenter.h for code", res);
    }
    else
    {
      ELLE_TRACE("mac unmount successful");
    }
    CFRunLoopStop(CFRunLoopGetCurrent());
  }

  void
  FuseContext::_mac_unmount(DurationOpt grace_time)
  {
    auto mountpoint = this->_mountpoint;
    reactor::background(
      [grace_time, mountpoint]
      {
        ELLE_TRACE("start mac unmount thread");
        DASessionRef session = DASessionCreate(kCFAllocatorDefault);
        CFURLRef path_url = CFURLCreateFromFileSystemRepresentation(
          kCFAllocatorDefault,
          reinterpret_cast<const unsigned char*>(mountpoint.data()),
          mountpoint.size(),
          true);
        DADiskRef disk = DADiskCreateFromVolumePath(
          kCFAllocatorDefault, session, path_url);
        CFRelease(path_url);
        ELLE_DEBUG("mac unmount disk: %s", (void*)(disk));
        if (disk)
        {
          DASessionScheduleWithRunLoop(
            session, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
          DADiskUnmount(disk,
                        kDADiskUnmountOptionForce,
                        _unmount_callback,
                        NULL);
          float run_time =
            grace_time ? grace_time.get().total_seconds() : 15.0f;
          // returns CFRunLoopRunResult on 10.11+
          ELLE_DEBUG("mac unmount start run loop");
          SInt32 res =
            CFRunLoopRunInMode(kCFRunLoopDefaultMode, run_time, false);
          if (res == kCFRunLoopRunTimedOut)
            ELLE_WARN("unmount run loop timed out");
          CFRelease(disk);
        }
        DASessionUnscheduleFromRunLoop(
          session, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
        CFRelease(session);
      });
  }
#endif
}
