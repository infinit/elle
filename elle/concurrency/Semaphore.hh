//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Semaphore.hh
//
// created       julien quintard   [sun mar  7 17:15:14 2010]
// updated       julien quintard   [wed mar 10 20:25:04 2010]
//

#ifndef ELLE_CONCURRENCY_SEMAPHORE_HH
#define ELLE_CONCURRENCY_SEMAPHORE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>

#include <elle/idiom/Close.hh>
# include <QSemaphore>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides a semaphore functionality.
    ///
    class Semaphore
    {
    public:
      //
      // constants
      //
      static const Natural32		Timeout = 0;

      //
      // methods
      //
      Status		Acquire(const Natural32 = 1,
				const Natural32 = Timeout);
      Status		Release(const Natural32 = 1);

      //
      // attributes
      //
      ::QSemaphore	semaphore;
    };

  }
}

#endif
