
#define FUSE_USE_VERSION 26
#define _FILE_OFFSET_BITS 64

#include "fuse.hh"

#include <fuse/fuse.h>
#include <fuse/fuse_lowlevel.h>

#include <boost/asio.hpp>

#include <elle/log.hh>
#include <elle/finally.hh>
#include <reactor/Barrier.hh>
#include <reactor/scheduler.hh>
#include <reactor/MultiLockBarrier.hh>
#include <reactor/exception.hh>

ELLE_LOG_COMPONENT("reactor.fuse");

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

  void FuseContext::loop_mt()
  {
    _loop.reset(new Thread("fuse loop",
                           [this] { this->_loop_mt();}));
  }
  void FuseContext::_loop_mt()
  {
    reactor::MultiLockBarrier barrier;
    fuse_session* s = fuse_get_session(_fuse);
    fuse_chan* ch = fuse_session_next_chan(s, NULL);
    size_t buffer_size = fuse_chan_bufsize(ch);
    int fd = fuse_chan_fd(ch);
    ELLE_TRACE("Got fuse fs %s", fd);
    boost::asio::posix::stream_descriptor socket(scheduler().io_service());
    socket.assign(fd);
    auto lock = _mt_barrier.lock();
    void* buffer_data = malloc(buffer_size);
    while (!fuse_exited(_fuse))
    {
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
      if (fuse_exited(_fuse))
        break;
      ELLE_DUMP("Processing command");
      fuse_buf fbuf { buffer_size, fuse_buf_flags(), buffer_data};
      int res = -EINTR;
      while(res == -EINTR)
        res = fuse_session_receive_buf(s, &fbuf, &ch);
      if (res == -EAGAIN)
        continue;
      if (res <= 0)
      {
        ELLE_LOG("%s: %s", res, strerror(-res));
        break;
      }
      void* b2 = malloc(buffer_size);
      memcpy(b2, buffer_data, buffer_size);
      fuse_buf fbuf2 = fbuf;
      fbuf2.mem = b2;

      _workers.push_back(new Thread("fuse worker", [s, fbuf2, ch, this] {
        auto lock = this->_mt_barrier.lock();
        fuse_session_process_buf(s, &fbuf2, ch);
        free(fbuf2.mem);
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
    args.argv = new char*[arguments.size() + 1];
    elle::SafeFinally cleanup([&] { delete[]args.argv;});
    for (int i=0; i<arguments.size(); ++i)
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
      _loop->terminate_now();
      for (auto const& t: _workers)
        t->terminate_now();
      reactor::wait(_mt_barrier);
    }
    ELLE_TRACE("done");
    fuse_session* s = ::fuse_get_session(_fuse);
    fuse_chan* ch = ::fuse_session_next_chan(s, NULL);
    ::fuse_unmount(_mountpoint.c_str(), ch);
    ::fuse_exit(_fuse);
    ::fuse_destroy(_fuse);
  }
}