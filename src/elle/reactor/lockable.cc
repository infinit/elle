#include <elle/assert.hh>
#include <elle/reactor/lockable.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/reactor/thread.hh>

namespace elle
{
  namespace reactor
  {
    Lock::Lock(Lockable& lockable)
      : _lockable(lockable)
    {
      auto sched = reactor::Scheduler::scheduler();
      ELLE_ASSERT(sched);
      auto current = sched->current();
      ELLE_ASSERT(current);
      while (!_lockable.acquire())
      {
        current->wait(_lockable);
      }
    }

    Lock::~Lock()
    {
      _lockable.release();
    }
  }
}
