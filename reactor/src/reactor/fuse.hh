#ifndef FUSE_HH
# define FUSE_HH

#include <string>
#include <vector>

#include <reactor/Barrier.hh>
#include <reactor/scheduler.hh>
#include <reactor/MultiLockBarrier.hh>

struct fuse;
struct fuse_operations;

namespace reactor
{
  class FuseContext
  {
  public:
    void create(std::string const& mountpoint,
                    std::vector<std::string> const& arguments,
                    const struct fuse_operations *op, size_t op_size,
                    void* user_data);
    void loop();
    void loop_mt();
    /// unmount and free ressources. Fore-kill after graceTime
    void destroy(DurationOpt graceTime = DurationOpt());
  private:
    void _loop_mt();
    fuse* _fuse;
    std::string _mountpoint;
    Barrier _socket_barrier;
    reactor::MultiLockBarrier _mt_barrier;
    std::unique_ptr<reactor::Thread> _loop;
    std::vector<reactor::Thread*> _workers;
  };

}

#endif