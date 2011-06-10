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
// updated       julien quintard   [fri jun 10 00:00:34 2011]
//

#ifndef ELLE_CONCURRENCY_CALLBACK_HH
#define ELLE_CONCURRENCY_CALLBACK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Parameters.hh>
#include <elle/radix/Base.hh>

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
    /// Callback generic class.
    ///
    template <typename... T>
    class Callback;

    ///
    /// this class represents a callback i.e a reference-based routine
    /// according to the given types.
    ///
    template <typename... T>
    class Callback< Parameters<T...> >:
      public Routine,
      public Base<Callback>
    {
    public:
      //
      // types
      //
      typedef Parameters<T...>		P;

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
      declare(Callback< Parameters<T...> >);

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
