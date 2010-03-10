//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Mutex.hh
//
// created       julien quintard   [sun mar  7 17:15:14 2010]
// updated       julien quintard   [wed mar 10 20:38:23 2010]
//

#ifndef ELLE_CONCURRENCY_MUTEX_HH
#define ELLE_CONCURRENCY_MUTEX_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>

#include <elle/idiom/Close.hh>
#include <QMutex>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides a mutex functionality.
    ///
    class Mutex
    {
    public:
      //
      // constants
      //
      static const Natural32		Timeout = 0;

      //
      // methods
      //
      Status		Lock(const Natural32 = Timeout);
      Status		Unlock();

      //
      // attributes
      //
      ::QMutex		mutex;
    };

  }
}

#endif
