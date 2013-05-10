#include <elle/system/signal.hh>

#include <signal.h>

#include <map>

namespace elle
{
  namespace system
  {
    // We don't use strsignal from libc because it's not portable.
    std::string
    strsignal(int signum)
    {
      static std::map<int, std::string> sigmap{
        {SIGABRT, "Aborted"},
        {SIGILL, "Illegal instruction"},
        {SIGFPE, "Floating point exception"},
        {SIGINT, "Interrupt"},
        {SIGSEGV, "Segmentation fault"},
        {SIGTERM, "Terminated"}
      };
      return sigmap[signum];
    }
  } /* system */
} /* elle */
