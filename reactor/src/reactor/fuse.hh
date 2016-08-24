#ifndef FUSE_HH
# define FUSE_HH

#include <string>
#include <vector>
#include <thread>

#include <reactor/Barrier.hh>
#include <reactor/scheduler.hh>
#include <reactor/MultiLockBarrier.hh>

struct fuse;
struct fuse_operations;

namespace reactor
{
  class FuseContext
  {
  /*-------------.
  | Construction |
  `-------------*/
  public:
    FuseContext();
    ~FuseContext();
    void
    create(std::string const& mountpoint,
           std::vector<std::string> const& arguments,
           const struct fuse_operations* op,
           size_t op_size,
           void* user_data);

  public:
    void
    loop();
    void
    loop_mt();
    void
    loop_pool(int threads);
    /// unmount and free ressources. Force-kill after "grace_time".
    void
    destroy(DurationOpt grace_time = DurationOpt());
    void
    kill();
    ELLE_ATTRIBUTE_RW(std::function<void()>, on_loop_exited);
  private:
    void
    _loop_single();
    void
    _loop_one_thread(Scheduler&);
    void
    _loop_mt(Scheduler&);
    void
    _loop_pool(int threads, Scheduler&);

#ifdef INFINIT_MACOSX
    void
    _mac_unmount(DurationOpt grace_time);
#endif

    fuse* _fuse;
    std::string _mountpoint;
    Barrier _socket_barrier;
    reactor::MultiLockBarrier _mt_barrier;
    std::unique_ptr<reactor::Thread> _loop;
    std::unique_ptr<std::thread> _loop_thread;
    std::vector<reactor::Thread*> _workers;
    std::mutex _mutex;
    MultiLockBarrier _destroy_barrier;
  };

}

#endif
