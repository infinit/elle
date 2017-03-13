#ifndef ELLE_SYSTEM_GETPID_HH
# define ELLE_SYSTEM_GETPID_HH

namespace elle
{
  namespace system
  {
    typedef int ProcessId;

    /// Portable way to get our PID.
    ProcessId
    getpid();

  }
}
#endif
