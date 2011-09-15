//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu feb  4 22:15:45 2010]
//

#ifndef ELLE_CONCURRENCY_FUNCTION_HH
#define ELLE_CONCURRENCY_FUNCTION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>
#include <elle/radix/Parameters.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace package;

  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// Function generic class.
    ///
    template <typename... T>
    class Function;

    ///
    /// this class represents a function.
    ///
    template <typename R,
	      typename... T>
    class Function< R,
		    Parameters<T...> >:
      public Object
    {
    public:
      //
      // types
      //
      typedef Parameters<T...>		P;

      //
      // types
      //
      typedef R				(*Handler)(T...);

      //
      // constructors & destructors
      //
      Function(Handler);
      Function(const Function<R, P>&);

      //
      // methods
      //
      R			Call(T...);
      Void		Trigger(T...);

      //
      // interfaces
      //

      // object
      declare(_(Function<R, P>));

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Handler		handler;
    };

    ///
    /// a specific class for function inference.
    ///
    template <>
    class Function<>
    {
    public:
      //
      // static methods
      //
      template <typename R,
		typename... T>
      static Function< R, Parameters<T...> >	Infer(R (*)(T...));
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/concurrency/Function.hxx>

#endif
