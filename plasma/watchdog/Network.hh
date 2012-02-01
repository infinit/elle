//
// ---------- header ----------------------------------------------------------
//
// project       plasma/watchdog
//
// license       infinit
//
// author        Raphaël Londeix   [Wed 01 Feb 2012 07:52:03 PM CET]
//

#ifndef PLASMA_WATCHDOG_NETWORK_HH
#define PLASMA_WATCHDOG_NETWORK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <string>

//
// ---------- classes ---------------------------------------------------------
//

namespace plasma
{
  namespace watchdog
  {

    ///
    /// Represent a user network
    ///
    struct Network
    {
      std::string id;
      std::string fullname;
    };

  }
}



#endif /* ! NETWORK_HH */


