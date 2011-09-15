//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu feb  4 22:18:05 2010]
//

#ifndef ELLE_CONCURRENCY_FUNCTION_HXX
#define ELLE_CONCURRENCY_FNUCTION_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    template <typename R,
	      typename... T>
    Function< R, Parameters<T...> >::Function(Handler		handler):
      handler(handler)
    {
    }

    ///
    /// the copy constructor.
    ///
    template <typename R,
	      typename... T>
    Function< R, Parameters<T...> >::Function(
				       const Function<
					 R,
				         Parameters<
					   T...
					   >
				         >&			function):
      Object(function),

      handler(function.handler)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method calls the handler.
    ///
    template <typename R,
	      typename... T>
    R
    Function< R, Parameters<T...> >::Call(T...			arguments)
    {
      return (this->handler(arguments...));
    }

    ///
    /// this method triggers the handler without checking the return value.
    ///
    template <typename R,
	      typename... T>
    Void
    Function< R, Parameters<T...> >::Trigger(T...		arguments)
    {
      this->handler(arguments...);
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(_(Function< R, Parameters<T...> >),
	  _(template <typename R,
		      typename... T>));

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the function state.
    ///
    template <typename R,
	      typename... T>
    Status
    Function< R, Parameters<T...> >::Dump(const Natural32	margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Function]" << std::endl;

      // dump the quantum.
      std::cout << alignment << Dumpable::Shift << "[Quantum] "
		<< std::dec << sizeof... (T) << std::endl;

      // dump the handler.
      std::cout << alignment << Dumpable::Shift << "[Handler] "
		<< std::hex << this->handler << std::endl;

      leave();
    }

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method returns a function of the type of the given handler.
    ///
    template <typename R,
	      typename... T>
    Function< R, Parameters<T...> >
    Function<>::Infer(R					(*handler)(T...))
    {
      return (Function< R, Parameters<T...> >(handler));
    }

  }
}

#endif
