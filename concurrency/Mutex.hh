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
// updated       julien quintard   [sun sep  4 15:29:56 2011]
//

#ifndef ELLE_CONCURRENCY_MUTEX_HH
#define ELLE_CONCURRENCY_MUTEX_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>

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
    /// this class provides a mutex functionality.
    ///
    /// \todo XXX this should be made fiber-friendly
    //
    class Mutex:
      public Entity
    {
    public:
      //
      // constants
      //
      static const Natural32		Timeout = 0;

      //
      // constructors & destructors
      //
      Mutex();

      //
      // methods
      //
      Status		Lock(const Natural32 = Timeout);
      Status		Unlock();

      //
      // attributes
      //
      ::QReadWriteLock	variable;
    };

  }
}

#endif
