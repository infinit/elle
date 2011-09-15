//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sun mar  7 17:15:14 2010]
//

#ifndef ELLE_CONCURRENCY_SEMAPHORE_HH
#define ELLE_CONCURRENCY_SEMAPHORE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>

#include <elle/idiom/Close.hh>
# include <QSemaphore>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;

  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides a semaphore functionality.
    ///
    class Semaphore:
      public Entity
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
