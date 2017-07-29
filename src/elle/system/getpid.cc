#include <elle/system/getpid.hh>

#ifdef ELLE_WINDOWS
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
#ifdef ELLE_WINDOWS
      return ::GetCurrentProcessId();
#else
      return ::getpid();
#endif
    }

  }
}
