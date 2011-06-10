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
// updated       julien quintard   [thu jun  9 18:22:11 2011]
//

#ifndef ELLE_CONCURRENCY_CLOSURE_HXX
#define ELLE_CONCURRENCY_CLOSURE_HXX

namespace elle
{
  namespace concurrency
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor.
    ///
    template <typename... T>
    Closure< Parameters<T...> >::Closure(Callback<
					   Closure<
					     Parameters<T...> >::P>& callback,
					   T&...		objects):
      callback(callback),
      arguments(objects...)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method triggers the closure by calling the callback with
    /// the pre-defined arguments.
    ///
    template <typename... T>
    Status	Closure< Parameters<T...> >::Trigger()
    {
      return (this->arguments.Trigger(this->callback));
    }

  }
}

#endif
