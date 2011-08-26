//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Morgue.hh
//
// created       julien quintard   [fri aug 26 17:04:49 2011]
// updated       julien quintard   [fri aug 26 17:56:38 2011]
//

#ifndef ELLE_RADIX_MORGUE_HH
#define ELLE_RADIX_MORGUE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Void.hh>
#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Meta.hh>

#include <elle/concurrency/Timer.hh>

#include <elle/idiom/Close.hh>
# include <list>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace concurrency;

  namespace radix
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Morgue:
      public Entity
    {
    public:
      //
      // constants
      //
      const static Natural32			Frequency;

      //
      // types
      //
      typedef std::list<Meta*>				Container;
      typedef typename Container::iterator		Iterator;
      typedef typename Container::const_iterator	Scoutor;

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      static Status	Instance(Morgue*&);

      //
      // static attributes
      //
      static Morgue*	Current;

      //
      // constructors & destructors
      //
      Morgue();
      ~Morgue();

      //
      // methods
      //
      template <typename T>
      Status		Register(T*);

      //
      // callbacks
      //
      Status		Bury();

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Container		container;
      Timer*		timer;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/radix/Morgue.hxx>

#endif
