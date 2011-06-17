//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Closure.hh
//
// created       julien quintard   [thu mar 25 03:25:25 2010]
// updated       julien quintard   [sat jun 18 00:32:49 2011]
//

#ifndef ELLE_CONCURRENCY_CLOSURE_HH
#define ELLE_CONCURRENCY_CLOSURE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>
#include <elle/radix/Parameters.hh>
#include <elle/radix/Arguments.hh>
#include <elle/radix/Base.hh>

#include <elle/concurrency/Callback.hh>

namespace elle
{
  using namespace radix;

  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// Closure generic class.
    ///
    /// note that this class is specialized in either a static or dynamic
    /// closure.
    ///
    template <typename... T>
    class Closure;

    ///
    /// this specialized class provides static closures i.e static arguments
    /// bound to a callback.
    ///
    /// once triggered, the arguments attached to the closure are passed
    /// to the callback.
    ///
    template <typename... U>
    class Closure< Parameters<U...> >:
      public Entity,
      public Base<Closure>
    {
    public:
      //
      // types
      //
      typedef Parameters<U...>		P;

      //
      // constructors & destructors
      //
      Closure(Callback< Parameters<U...> >&,
	      U&...);

      //
      // methods
      //
      Status		Trigger();

      //
      // attributes
      //
      Callback< Parameters<U...> >	callback;
      Arguments< Parameters<U...> >	arguments;
    };

    ///
    /// this specialized class provides dynamic closures. unlike static
    /// closures, both static and dynamic arguments can be provided.
    ///
    /// for instance, the closure:
    ///
    ///   Closure< Parameters<Natural32>,
    ///            Parameters<String> > closure(callback, n);
    ///
    /// takes a Natural32 (n) as a static argument i.e at the closure creation
    /// but also takes a String once the closure is triggered. thus,
    /// such a closure would be used as follows:
    ///
    ///   closure.Trigger(string);
    ///
    /// such a call would result in the callback being triggered with both
    /// the static and dynamic arguments. in this example, the callback
    /// would have to be of the type:
    ///
    ///   Callback< Parameters<Natural32, String> >
    ///
    template <typename... U,
	      typename... V>
    class Closure< Parameters<U...>,
		   Parameters<V...> >:
      public Entity,
      public Base<Closure>
    {
    public:
      //
      // types
      //
      typedef Parameters<U..., V...>	P;

      //
      // constructors & destructors
      //
      Closure(Callback< Parameters<U..., V...> >&,
	      U&...);

      //
      // methods
      //
      Status		Trigger(V&...);

      //
      // attributes
      //
      Callback< Parameters<U..., V...> >	callback;
      Arguments< Parameters<U...> >		arguments;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/concurrency/Closure.hxx>

#endif
