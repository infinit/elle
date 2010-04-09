//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/miscellaneous/Callback.hh
//
// created       julien quintard   [wed mar 24 15:49:05 2010]
// updated       julien quintard   [tue mar 30 17:28:25 2010]
//

#ifndef ELLE_MISCELLANEOUS_CALLBACK_HH
#define ELLE_MISCELLANEOUS_CALLBACK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/miscellaneous/Status.hh>
#include <elle/miscellaneous/Routine.hh>
#include <elle/miscellaneous/Function.hh>
#include <elle/miscellaneous/Method.hh>

namespace elle
{
  namespace miscellaneous
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents an callback i.e a reference-based routine.
    ///
    template <typename... T>
    class Callback:
      public Routine
    {
    public:
      //
      // constructors & destructors
      //
      Callback();
      Callback(typename Function<T&...>::Handler);
      template <typename C>
      Callback(typename Method<T&...>:: template Wrap<C>::Handler,
	       C*);
      Callback(const Callback&);
      ~Callback();

      //
      // methods
      //
      Status		Trigger(T&...) const;

      //
      // interfaces
      //

      // entity
      declare(Entity, Callback<T...>);
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

#include <elle/miscellaneous/Callback.hxx>

#endif
