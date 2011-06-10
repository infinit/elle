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
// updated       julien quintard   [fri jun 10 00:01:19 2011]
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
    template <typename... T>
    class Closure;

    ///
    /// this class provides closures i.e variables bound to a callback.
    ///
    template <typename... T>
    class Closure< Parameters<T...> >:
      public Entity,
      public Base<Closure>
    {
    public:
      //
      // types
      //
      typedef Parameters<T...>		P;

      //
      // constructors & destructors
      //
      Closure(Callback<P>&,
	      T&...);

      //
      // methods
      //
      Status		Trigger();

      //
      // attributes
      //
      Callback<P>	callback;
      Arguments<P>	arguments;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/concurrency/Closure.hxx>

#endif
