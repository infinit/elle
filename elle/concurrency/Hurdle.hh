//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien.quintard   [tue oct 25 12:35:02 2011]
//

#ifndef ELLE_CONCURRENCY_HH
#define ELLE_CONCURRENCY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Void.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

#include <elle/concurrency/Mode.hh>

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
    class Hurdle:
      public Object
    {
    public:
      //
      // static methods
      //
      // XXX

      //
      // constructors & destructors
      //
      Hurdle();

      //
      // methods
      //
      Void		Lock(const Mode);
      Void		Unlock(const Mode);

      Status		Try(const Mode);

      //
      // interfaces
      //

      // XXX object

      //
      // attributes
      //
      Boolean		locked;
      Natural32		readers;
    };

  }
}

#endif
