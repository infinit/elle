#pragma once

namespace elle
{
  namespace system
  {
    using ProcessId = int;

    /// Portable way to get our PID.
    ProcessId
    getpid();

  }
}
