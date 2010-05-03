//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Entrance.hh
//
// created       julien quintard   [wed mar 24 15:49:05 2010]
// updated       julien quintard   [mon may  3 22:30:45 2010]
//

#ifndef ELLE_CONCURRENCY_ENTRANCE_HH
#define ELLE_CONCURRENCY_ENTRANCE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>

#include <elle/concurrency/Routine.hh>
#include <elle/concurrency/Function.hh>
#include <elle/concurrency/Method.hh>

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

      // object
      declare(Entrance<T...>, _());

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

#include <elle/concurrency/Entrance.hxx>

#endif
