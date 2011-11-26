//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sun mar  7 17:15:14 2010]
//

#ifndef ELLE_CONCURRENCY_MUTEX_HH
#define ELLE_CONCURRENCY_MUTEX_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Boolean.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>
#include <elle/radix/Parameters.hh>

#include <elle/concurrency/Closure.hh>
#include <elle/concurrency/Section.hh>

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
    /// this class represents the simplest concurrency control mechanism:
    /// the mutex.
    ///
    /// the idea behind such a mechanism is to let authorize the access
    /// to an object in an exclusive way.
    ///
    /// note that this concurrency mechanism has been designed for
    /// fibers exclusively and would therefore not work with threads.
    ///
    /// besides, an helper class Zone is provided which combines the
    /// Mutex with a Section in order to make sure the mutex is always
    /// unlocked when leaving the scope. this class can be used as follows:
    ///
    ///   Mutex			mutex;
    ///   Mutex::Zone		zone(mutex);
    ///
    ///   zone.Lock();
    ///
    ///   [do something]
    ///
    ///   if ([something else])
    ///     escape("in this case, the mutex will be automatically unlocked");
    ///
    ///   zone.Unlock();
    ///
    class Mutex:
      public Object
    {
    public:
      //
      // types
      //
      typedef Section<
        Parameters<>,
        Parameters<>
        >							S;
      typedef Closure<
	Void,
	Parameters<>,
	Parameters<>
	>							L;
      typedef Closure<
	Void,
	Parameters<>,
	Parameters<>
	>							U;
      typedef Callback<
	Void,
	Parameters<>
	>							C;

      //
      // classes
      //

      ///
      /// this class makes it easy to make sure a mutex is always unlocked
      /// when leaving a scope.
      ///
      class Zone:
	public Entity
      {
      public:
	//
	// constructors & destructors
	//
	Zone(Mutex&);

	//
	// methods
	//
	Void		Lock();
	Void		Unlock();

	//
	// interfaces
	//

	// dumpable
	Status		Dump(const Natural32 = 0) const;

	//
	// attributes
	//
	Mutex&		mutex;
	S		section;
      };

      //
      // constructors & destructors
      //
      Mutex();

      //
      // methods
      //
      Void		Lock();
      Void		Unlock();

      Status		Try();

      //
      // interfaces
      //

      // object
      declare(Mutex);

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Boolean		locked;
    };

  }
}

#endif
