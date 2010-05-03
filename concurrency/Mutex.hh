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
// updated       julien quintard   [mon may  3 21:13:05 2010]
//

#ifndef ELLE_CONCURRENCY_MUTEX_HH
#define ELLE_CONCURRENCY_MUTEX_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>

#include <elle/concurrency/Resource.hh>

#include <elle/idiom/Close.hh>
# include <QMutex>
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
    /// note that mutexes provide thread-fiber-safety.
    ///
    /// \todo XXX the other concurrency control classes such as condition
    /// semaphore etc. should inherit from Resource
    ///
    class Mutex:
      public Resource
    {
    public:
      //
      // enumerations
      //
      enum State
	{
	  StateUnlocked,
	  StateLocked
	};

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
      ::QMutex		mutex;
      State		state;
    };

  }
}

#endif
