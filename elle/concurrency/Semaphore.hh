//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/concurrency/Semaphore.hh
//
// created       julien quintard   [sun mar  7 17:15:14 2010]
// updated       julien quintard   [mon apr 26 19:12:01 2010]
//

#ifndef ELLE_CONCURRENCY_SEMAPHORE_HH
#define ELLE_CONCURRENCY_SEMAPHORE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

#include <elle/miscellaneous/Status.hh>

#include <elle/idiom/Close.hh>
# include <QSemaphore>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace miscellaneous;

  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides a semaphore functionality.
    ///
    class Semaphore:
      public Meta
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
