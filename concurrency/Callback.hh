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
// updated       julien quintard   [fri sep  2 22:27:14 2011]
//

#ifndef ELLE_CONCURRENCY_CALLBACK_HH
#define ELLE_CONCURRENCY_CALLBACK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Parameters.hh>

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
    template <typename R,
	      typename... T>
    class Callback< R,
		    Parameters<T...> >:
      public Routine
    {
    public:
      //
      // constants
      //
      static const Callback< R,
			     Parameters<T...> >		Null;

      //
      // types
      //
      typedef Parameters<T...>		P;

      //
      // constructors & destructors
      //
      Callback();
      Callback(typename Function<R, P>::Handler);
      template <typename C>
      Callback(typename Method<R, P>::template Wrap<C>::Handler,
	       C*);
      Callback(const Callback<R, P>&);
      ~Callback();

      //
      // methods
      //
      R			Call(T...) const;
      Void		Trigger(T...) const;

      //
      // interfaces
      //

      // object
      declare(_(Callback< R, Parameters<T...> >));

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      union
      {
	Function<R, P>*		function;
	Method<R, P>*		method;
      };
    };

    ///
    /// a specific class for callback inference.
    ///
    template <>
    class Callback<>
    {
    public:
      //
      // static methods
      //
      template <typename R,
		typename... T>
      static Callback< R, Parameters<T...> >	Infer(R (*)(T...));
      template <typename R,
		typename C,
		typename... T>
      static Callback< R, Parameters<T...> >	Infer(R (C::*)(T...),
						      C*);
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/concurrency/Callback.hxx>

#endif
