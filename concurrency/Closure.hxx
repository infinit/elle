//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Closure.hxx
//
// created       julien quintard   [thu mar 25 11:55:10 2010]
// updated       julien quintard   [sat jun 18 00:35:26 2011]
//

#ifndef ELLE_CONCURRENCY_CLOSURE_HXX
#define ELLE_CONCURRENCY_CLOSURE_HXX

namespace elle
{
  namespace concurrency
  {

//
// ---------- static closure --------------------------------------------------
//

    ///
    /// the constructor.
    ///
    template <typename... U>
    Closure< Parameters<U...> >::Closure(
        Callback< Parameters<U...> >&				callback,
	U&...							objects):
      callback(callback),
      arguments(objects...)
    {
    }

    ///
    /// this method triggers the closure by calling the callback with
    /// the pre-defined arguments.
    ///
    template <typename... U>
    Status	Closure< Parameters<U...> >::Trigger()
    {
      return (this->arguments.Trigger(this->callback));
    }

//
// ---------- dynamic closure -------------------------------------------------
//

    ///
    /// the constructor.
    ///
    template <typename... U,
	      typename... V>
    Closure< Parameters<U...>,
	     Parameters<V...> >::Closure(
        Callback< Parameters<U..., V...> >&			callback,
	U&...							objects):
      callback(callback),
      arguments(objects...)
    {
    }

    ///
    /// this method triggers the closure by calling the callback with
    /// the pre-defined arguments.
    ///
    template <typename... U,
	      typename... V>
    Status	Closure< Parameters<U...>,
			 Parameters<V...> >::Trigger(V&...	objects)
    {
      return (this->arguments.Trigger(this->callback, objects...));
    }

  }
}

#endif
