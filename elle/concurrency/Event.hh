//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Event.hh
//
// created       julien quintard   [sun mar 14 12:58:10 2010]
// updated       julien quintard   [sun mar 14 13:51:56 2010]
//

#ifndef ELLE_CONCURRENCY_EVENT_HH
#define ELLE_CONCURRENCY_EVENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Misc.hh>

#include <elle/concurrency/Semaphore.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Event
    {
    public:
      //
      // static methods
      //
      static Status	Process();
      static Status	Wait(Semaphore&);
    };

  }
}

#endif
