//
// ---------- header ----------------------------------------------------------
//
// project       @FIXME@
//
// license       infinit
//
// author        @NAME@   [Thu 01 Mar 2012 01:50:30 PM CET]
//

#ifndef AGENT_HH
#define AGENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <memory>

namespace plasma
{
  namespace watchdog
  {

    class WatchdogInterface;

//
// ---------- classes ---------------------------------------------------------
//

    class Agent
    {
    private:
      std::unique_ptr<WatchdogInterface>  _interface;

    public:
      Agent(std::unique_ptr<WatchdogInterface>&& interface);
      void Run();
    };

  }
}

#endif /* ! AGENT_HH */


