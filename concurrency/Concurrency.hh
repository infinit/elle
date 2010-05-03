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
// updated       julien quintard   [mon may  3 21:12:51 2010]
//

#ifndef ELLE_CONCURRENCY_CONCURRENCY_HH
#define ELLE_CONCURRENCY_CONCURRENCY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Status.hh>

namespace elle
{
  using namespace radix;

  ///
  /// this namespace contains functionalities for concurrency purposes.
  ///
  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides control over the concurrency module.
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
#include <elle/concurrency/Callback.hh>
#include <elle/concurrency/Closure.hh>
#include <elle/concurrency/Condition.hh>
#include <elle/concurrency/Entrance.hh>
#include <elle/concurrency/Environment.hh>
#include <elle/concurrency/Event.hh>
#include <elle/concurrency/Fiber.hh>
#include <elle/concurrency/Frame.hh>
#include <elle/concurrency/Function.hh>
#include <elle/concurrency/Method.hh>
#include <elle/concurrency/Mode.hh>
#include <elle/concurrency/Mutex.hh>
#include <elle/concurrency/Phase.hh>
#include <elle/concurrency/Program.hh>
#include <elle/concurrency/Resource.hh>
#include <elle/concurrency/Routine.hh>
#include <elle/concurrency/Semaphore.hh>
#include <elle/concurrency/Thread.hh>
#include <elle/concurrency/Timer.hh>

#endif
