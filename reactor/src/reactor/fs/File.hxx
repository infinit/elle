#ifndef REACTOR_FS_FILE_HXX
# define REACTOR_FS_FILE_HXX

namespace reactor
{
  namespace fs
  {
    template<typename... Args>
    File::File(Scheduler& sched,
               Args&&... args):
      _scheduler(sched),
      _file{sched.io_service(), std::forward<Args>(args)...}
    {}
  }
}

#endif
