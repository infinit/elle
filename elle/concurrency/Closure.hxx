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
// updated       julien quintard   [mon jul 18 11:43:36 2011]
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
    template <typename R,
	      typename... U>
    Closure< R, Parameters<U...> >::Closure(
        Callback< R, Parameters<U...> >&			callback,
	U&...							objects):
      callback(callback),
      arguments(objects...)
    {
    }

    ///
    /// this method calls the closure by calling the callback with
    /// the pre-defined arguments.
    ///
    template <typename R,
	      typename... U>
    R		Closure< R, Parameters<U...> >::Call()
    {
      return (this->arguments.Call(this->callback));
    }

    ///
    /// this method triggers the closure without checking the return value.
    ///
    template <typename R,
	      typename... U>
    Void	Closure< R, Parameters<U...> >::Trigger()
    {
      this->arguments.Trigger(this->callback);
    }

//
// ---------- dynamic closure -------------------------------------------------
//

    ///
    /// the constructor.
    ///
    template <typename R,
	      typename... U,
	      typename... V>
    Closure< R,
	     Parameters<U...>,
	     Parameters<V...> >::Closure(
	Callback< R, Parameters<U..., V...> >&			callback,
	U&...							objects):
      callback(callback),
      arguments(objects...)
    {
    }

    ///
    /// this method calls the closure by calling the callback with
    /// the pre-defined arguments.
    ///
    template <typename R,
	      typename... U,
	      typename... V>
    R		Closure< R,
			 Parameters<U...>,
			 Parameters<V...> >::Call(V&...		objects)
    {
      return (this->arguments.Trigger(this->callback, objects...));
    }

    ///
    /// this method triggers the closure without checking the return value.
    ///
    template <typename R,
	      typename... U,
	      typename... V>
    Void	Closure< R,
			 Parameters<U...>,
			 Parameters<V...> >::Trigger(V&...	objects)
    {
      this->arguments.Trigger(this->callback, objects...);
    }

  }
}

#endif
