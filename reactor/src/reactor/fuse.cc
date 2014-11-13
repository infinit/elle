
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

ELLE_LOG_COMPONENT("reactor.fuse");

namespace reactor
{
  void fuse_loop(fuse* f)
  {
    fuse_session* s = fuse_get_session(f);
    fuse_chan* ch = fuse_session_next_chan(s, NULL);
    int fd = fuse_chan_fd(ch);
    ELLE_TRACE("Got fuse fs %s", fd);
    boost::asio::posix::stream_descriptor socket(scheduler().io_service());
    socket.assign(fd);
    Barrier b;

    while (!fuse_exited(f))
    {
      b.close();
      socket.async_read_some(boost::asio::null_buffers(),
        [&](boost::system::error_code const&, std::size_t)
        {
          b.open();
        });
      ELLE_DUMP("waiting for socket");
      wait(b);
      ELLE_DUMP("Processing command");
      //highlevel api
      fuse_cmd* cmd = fuse_read_cmd(f);
      if (!cmd)
        break;
      fuse_process_cmd(f, cmd);
    }
    socket.release();
  }

  void fuse_loop_mt(fuse* f)
  {
    fuse_session* s = fuse_get_session(f);
    fuse_chan* ch = fuse_session_next_chan(s, NULL);
    size_t buffer_size = fuse_chan_bufsize(ch);
    int fd = fuse_chan_fd(ch);
    ELLE_TRACE("Got fuse fs %s", fd);
    boost::asio::posix::stream_descriptor socket(scheduler().io_service());
    socket.assign(fd);
    Barrier b;
    
    void* buffer_data = malloc(buffer_size);
    while (!fuse_exited(f))
    {
      b.close();
      socket.async_read_some(boost::asio::null_buffers(),
        [&](boost::system::error_code const&, std::size_t)
        {
          b.open();
        });
      ELLE_DUMP("waiting for socket");
      wait(b);
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

      new Thread("fuse worker", [s, fbuf2, ch] {
        fuse_session_process_buf(s, &fbuf2, ch);
        free(fbuf2.mem);
      });
    }
  }

  fuse* fuse_create(std::string const& mountpoint,
                    std::vector<std::string> const& arguments,
                    const struct fuse_operations *op, size_t op_size,
                    void* user_data)
  {
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
    fuse* res = ::fuse_new(chan, &args, op, op_size, user_data);
    return res;
  }
  void fuse_destroy(fuse* f, std::string const& mountpoint)
  {   
    fuse_session* s = ::fuse_get_session(f);
    fuse_chan* ch = ::fuse_session_next_chan(s, NULL);
    ::fuse_unmount(mountpoint.c_str(), ch);
    ::fuse_destroy(f);
  }
}