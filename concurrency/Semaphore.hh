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
    /// this class represents a semaphore from which resources can be
    /// acquired or released.
    ///
    /// note that this concurrency mechanism has been designed for
    /// fibers exclusively and would therefore not work with threads.
    ///
    /// besides, an helper class Zone is provided which combines the
    /// Semaphore with a Section in order to make sure the semaphore is always
    /// unlocked when leaving the scope. this class can be used as follows:
    ///
    ///   Semaphore             semaphore;
    ///   Semaphore::Zone       zone(semaphore);
    ///
    ///   zone.Lock();
    ///
    ///   [do something]
    ///
    ///   if ([something else])
    ///     escape("in this case, the semaphore will be automatically "
    ///            "unlocked");
    ///
    ///   zone.Unlock();
    ///
    class Semaphore:
      public Object
    {
    public:
    public:
      //
      // types
      //
      typedef Section<
        Parameters<const Natural32>,
        Parameters<const Natural32>
        >                                                       S;
      typedef Closure<
        Void,
        Parameters<const Natural32>,
        Parameters<>
        >                                                       A;
      typedef Closure<
        Void,
        Parameters<const Natural32>,
        Parameters<>
        >                                                       R;
      typedef Callback<
        Void,
        Parameters<const Natural32>
        >                                                       C;

      //
      // classes
      //

      ///
      /// this class makes it easy to make sure resources always get released
      /// from the semaphore when leaving a scope.
      ///
      class Zone:
        public Entity
      {
      public:
        //
        // constructors & destructors
        //
        Zone(Semaphore&,
             const Natural32);

        //
        // methods
        //
        Void            Acquire();
        Void            Release();

        //
        // interfaces
        //

        // dumpable
        Status          Dump(const Natural32 = 0) const;

        //
        // attributes
        //
        Semaphore&      semaphore;
        Natural32       n;

        S               section;
      };

      //
      // constructors & destructors
      //
      Semaphore(const Natural32 = 0);

      //
      // methods
      //
      Void              Acquire(const Natural32);
      Void              Release(const Natural32);

      Status            Try(const Natural32);

      //
      // interfaces
      //

      // object
      declare(Semaphore);

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Natural32         acquired;
      Natural32         available;
    };

  }
}

#endif
