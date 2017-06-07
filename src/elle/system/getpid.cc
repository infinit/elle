#include <elle/system/getpid.hh>

#ifdef INFINIT_WINDOWS
# include <elle/windows.hh>
#else
# include <unistd.h>
#endif

namespace elle
{
  namespace system
  {

    ProcessId
    getpid()
    {
#ifdef INFINIT_WINDOWS
      return ::GetCurrentProcessId();
#else
      return ::getpid();
#endif
    }

  }
}
