//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Accord.hh
//
// created       julien quintard   [sun mar  7 17:15:14 2010]
// updated       julien quintard   [sat jun 18 15:31:40 2011]
//

#ifndef ELLE_CONCURRENCY_ACCORD_HH
#define ELLE_CONCURRENCY_ACCORD_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>

#include <elle/concurrency/Mode.hh>

#include <elle/idiom/Close.hh>
# include <QReadWriteLock>
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
    /// this class provides an accord functionality i.e the possibility
    /// to access a resource in reading or writing such that multi read
    /// accesses are authorised but a single write access is exclusive to
    /// the others.
    ///
    /// XXX what is this class for? threads/fibers?
    ///
    class Accord:
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
      Status		Lock(const Mode,
			     const Natural32 = Timeout);
      Status		Unlock();

      //
      // attributes
      //
      ::QReadWriteLock	accord;
    };

  }
}

#endif
