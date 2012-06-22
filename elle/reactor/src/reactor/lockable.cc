#include <reactor/lockable.hh>
#include <reactor/thread.hh>

namespace reactor
{
  Lock::Lock(Scheduler& sched, Lockable& lockable)
    : _lockable(lockable)
  {
    sched.current()->wait(_lockable);
  }

  Lock::~Lock()
  {
    _lockable.release();
  }
}
