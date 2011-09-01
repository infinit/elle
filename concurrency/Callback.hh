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
// updated       julien quintard   [thu sep  1 14:20:30 2011]
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
      // types
      //
      typedef Parameters<T...>		P;

      //
      // constructors & destructors
      //
      Callback();
      Callback(typename Function< R, Parameters<T&...> >::Handler);
      template <typename C>
      Callback(typename Method< R, Parameters<T&...> >::
		 template Wrap<C>::Handler,
	       C*);
      Callback(const Callback< R, Parameters<T...> >&);
      ~Callback();

      //
      // methods
      //
      R			Call(T&...) const;
      Void		Trigger(T&...) const;

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
      typename Routine::Scheme		scheme;

      union
      {
	Function< R,
		  Parameters<T&...> >*	function;
	Method< R,
		Parameters<T&...> >*	method;
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
