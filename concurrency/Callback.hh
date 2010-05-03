//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Callback.hh
//
// created       julien quintard   [wed mar 24 15:49:05 2010]
// updated       julien quintard   [mon may  3 22:30:54 2010]
//

#ifndef ELLE_CONCURRENCY_CALLBACK_HH
#define ELLE_CONCURRENCY_CALLBACK_HH

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

      // object
      declare(Callback<T...>, _());

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

#include <elle/concurrency/Callback.hxx>

#endif
