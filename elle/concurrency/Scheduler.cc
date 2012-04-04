#include <elle/concurrency/Scheduler.hh>

namespace elle
{
  namespace concurrency
  {
    reactor::Scheduler&
    scheduler()
    {
      static reactor::Scheduler* res = 0;
      if (res == 0)
        res = new reactor::Scheduler;
      return *res;
    }
  }
}
