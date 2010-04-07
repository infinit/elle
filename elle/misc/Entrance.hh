//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/misc/Entrance.hh
//
// created       julien quintard   [wed mar 24 15:49:05 2010]
// updated       julien quintard   [tue mar 30 17:28:37 2010]
//

#ifndef ELLE_MISC_ENTRANCE_HH
#define ELLE_MISC_ENTRANCE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Status.hh>
#include <elle/misc/Routine.hh>
#include <elle/misc/Function.hh>
#include <elle/misc/Method.hh>

namespace elle
{
  namespace misc
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents an entrance i.e a method that is launched
    /// in a new fiber.
    ///
    /// note that if such an entrance returns an error, the report is
    /// displayed as this is interpreted as an extremely abnormal situation.
    ///
    template <typename... T>
    class Entrance:
      public Routine
    {
    public:
      //
      // constructors & destructors
      //
      Entrance();
      Entrance(typename Function<T&...>::Handler);
      template <typename C>
      Entrance(typename Method<T&...>:: template Wrap<C>::Handler,
	       C*);
      Entrance(const Entrance&);
      ~Entrance();

      //
      // methods
      //
      Status		Trigger(T&...) const;

      //
      // interfaces
      //

      // entity
      declare(Entity, Entrance<T...>);
      // XXX

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      typename Routine::Scheme	scheme;

      union
      {
	Function<T&...>*	function;
	Method<T&...>*		method;
      };
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/misc/Entrance.hxx>

#endif
