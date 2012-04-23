#include <reactor/debug.hh>
#include <reactor/scheduler.hh>

namespace reactor
{
  namespace debug
  {
    boost::mutex debug_mutex;

    void Dump(Scheduler& s)
    {
      s.Dump(std::cerr);
    }
  }
}
