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
// updated       julien quintard   [thu mar 25 00:30:47 2010]
//

#ifndef ELLE_CONCURRENCY_MUTEX_HH
#define ELLE_CONCURRENCY_MUTEX_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

#include <elle/misc/Status.hh>

#include <elle/idiom/Close.hh>
#include <QMutex>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace misc;

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
