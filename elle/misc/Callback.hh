//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/misc/Callback.hh
//
// created       julien quintard   [wed mar 24 15:49:05 2010]
// updated       julien quintard   [thu mar 25 18:10:24 2010]
//

#ifndef ELLE_MISC_CALLBACK_HH
#define ELLE_MISC_CALLBACK_HH

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
      Status		Call(T&...);

      //
      // interfaces
      //

      // entity
      declare(Entity, Callback);
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

#include <elle/misc/Callback.hxx>

#endif
