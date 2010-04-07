//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Concurrency.hh
//
// created       julien quintard   [sun mar  7 17:14:02 2010]
// updated       julien quintard   [wed mar 31 21:14:49 2010]
//

#ifndef ELLE_CONCURRENCY_CONCURRENCY_HH
#define ELLE_CONCURRENCY_CONCURRENCY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Status.hh>

namespace elle
{
  using namespace misc;

  ///
  /// this namespace contains functionalities for concurrency purposes.
  ///
  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Concurrency
    {
    public:
      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Accord.hh>
#include <elle/concurrency/Condition.hh>
#include <elle/concurrency/Mode.hh>
#include <elle/concurrency/Mutex.hh>
#include <elle/concurrency/Program.hh>
#include <elle/concurrency/Semaphore.hh>
#include <elle/concurrency/Thread.hh>
#include <elle/concurrency/Timer.hh>
#include <elle/concurrency/Fiber.hh>
#include <elle/concurrency/Event.hh>

#endif
