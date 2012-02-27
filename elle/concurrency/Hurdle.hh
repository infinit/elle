//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue oct 25 12:35:02 2011]
//

#ifndef ELLE_CONCURRENCY_HH
#define ELLE_CONCURRENCY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Void.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>
#include <elle/radix/Parameters.hh>

#include <elle/concurrency/Mode.hh>
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
    /// this class represents a read-write mutex for which the caller
    /// indicates its mode i.e read or write.
    ///
    /// the idea behind such a concurrency control object is to let
    /// readers access the object concurrently while allowing a single writer
    /// i.e everyone gets locked should a writer acquire the hurdle.
    ///
    /// note that this concurrency mechanism has been designed for
    /// fibers exclusively and would therefore not work with threads.
    ///
    /// besides, an helper class Zone is provided which combines the
    /// Hurdle with a Section in order to make sure the hurdle is always
    /// unlocked when leaving the scope. this class can be used as follows:
    ///
    ///   Hurdle                hurdle;
    ///   Hurdle::Zone          zone(hurdle, ModeWrite);
    ///
    ///   zone.Lock();
    ///
    ///   [do something]
    ///
    ///   if ([something else])
    ///     escape("in this case, the hurdle will be automatically unlocked");
    ///
    ///   zone.Unlock();
    ///
    class Hurdle:
      public Object
    {
    public:
      //
      // types
      //
      typedef Section<
        Parameters<const Mode>,
        Parameters<const Mode>
        >                                                       S;
      typedef Closure<
        Void,
        Parameters<const Mode>,
        Parameters<>
        >                                                       L;
      typedef Closure<
        Void,
        Parameters<const Mode>,
        Parameters<>
        >                                                       U;
      typedef Callback<
        Void,
        Parameters<const Mode>
        >                                                       C;

      //
      // classes
      //

      ///
      /// this class makes it easy to make sure a hurdle is always unlocked
      /// when leaving a scope.
      ///
      class Zone:
        public Entity
      {
      public:
        //
        // constructors & destructors
        //
        Zone(Hurdle&,
             const Mode);

        //
        // methods
        //
        Void            Lock();
        Void            Unlock();

        //
        // interfaces
        //

        // dumpable
        Status          Dump(const Natural32 = 0) const;

        //
        // attributes
        //
        Hurdle&         hurdle;
        Mode            mode;

        S               section;
      };

      //
      // constructors & destructors
      //
      Hurdle();

      //
      // methods
      //
      Void              Lock(const Mode);
      Void              Unlock(const Mode);

      Status            Try(const Mode);

      //
      // interfaces
      //

      // object
      declare(Hurdle);

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Boolean           locked;
      Natural32         readers;
    };

  }
}

#endif
