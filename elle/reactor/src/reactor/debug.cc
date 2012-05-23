#include <reactor/debug.hh>
#include <reactor/scheduler.hh>

namespace reactor
{
  namespace debug
  {
    boost::mutex debug_mutex;

    void dump(Scheduler& s)
    {
      s.dump(std::cerr);
    }
  }
}
