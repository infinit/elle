//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Callback.hxx
//
// created       julien quintard   [wed mar 24 23:43:50 2010]
// updated       julien quintard   [sun jun 19 22:27:22 2011]
//

#ifndef ELLE_CONCURRENCY_CALLBACK_HXX
#define ELLE_CONCURRENCY_CALLBACK_HXX

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
    /// default constructor.
    ///
    template <typename... T>
    Callback< Parameters<T...> >::Callback():
      Routine::Routine(Routine::TypeCallback),

      scheme(Routine::SchemeUnknown)
    {
    }

    ///
    /// function-based constructor.
    ///
    template <typename... T>
    Callback< Parameters<T...> >::Callback(typename
					     Function<T&...>::Handler handler):
      Routine::Routine(Routine::TypeCallback),

      scheme(Routine::SchemeFunction),
      function(new Function<T&...>(handler))
    {
    }

    ///
    /// method-based constructor.
    ///
    template <typename... T>
    template <typename C>
    Callback< Parameters<T...> >::Callback(typename
 					     Method<T&...>::
					       template Wrap<C>::Handler
					         handler,
					   C*			object):
      Routine::Routine(Routine::TypeCallback),

      scheme(Routine::SchemeMethod),
      method(new Method<T&...>(handler, object))
    {
    }

    ///
    /// copy constructor.
    ///
    template <typename... T>
    Callback< Parameters<T...> >::Callback(const Callback&	callback):
      Routine(callback),

      scheme(callback.scheme)
    {
      enter();

      // copy the content depending on the scheme.
      switch (this->scheme)
	{
	case Routine::SchemeFunction:
	  {
	    // clone the function.
	    this->function = new Function<T&...>(*callback.function);

	    break;
	  }
	case Routine::SchemeMethod:
	  {
	    // clone the method.
	    this->method = new Method<T&...>(*callback.method);

	    break;
	  }
	case Routine::SchemeUnknown:
	  {
	    break;
	  }
	}

      release();
    }

    ///
    /// destructor.
    ///
    template <typename... T>
    Callback< Parameters<T...> >::~Callback()
    {
      // release the the content.
      switch (this->scheme)
	{
	case Routine::SchemeFunction:
	  {
	    // delete the function.
	    delete this->function;

	    break;
	  }
	case Routine::SchemeMethod:
	  {
	    // delete the method.
	    delete this->method;

	    break;
	  }
	case Routine::SchemeUnknown:
	  {
	    break;
	  }
	}
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Callback< Parameters<T...> >, _(template <typename... T>));

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the callback.
    ///
    template <typename... T>
    Status	Callback< Parameters<T...> >::Dump(const Natural32 margin)
      const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Callback]" << std::endl;

      // dump the scheme.
      std::cout << alignment << Dumpable::Shift << "[Scheme] "
		<< this->scheme << std::endl;

      // dump the content.
      switch (this->scheme)
	{
	case Routine::SchemeFunction:
	  {
	    // dump the function.
	    if (this->function->Dump(margin + 2) == StatusError)
	      escape("unable to dump the function");

	    break;
	  }
	case Routine::SchemeMethod:
	  {
	    // dump the method.
	    if (this->method->Dump(margin + 2) == StatusError)
	      escape("unable to dump the method");

	    break;
	  }
	case Routine::SchemeUnknown:
	  {
	    escape("unknown scheme");
	  }
	}

      leave();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method calls the callback implementation.
    ///
    template <typename... T>
    Status	Callback< Parameters<T...> >::Trigger(T&...	arguments)
      const
    {
      enter();

      // call depending on the scheme.
      switch (this->scheme)
	{
	case Routine::SchemeFunction:
	  {
	    // call the function.
	    if (this->function->Call(arguments...) == StatusError)
	      escape("unable to call the function");

	    break;
	  }
	case Routine::SchemeMethod:
	  {
	    // call the method.
	    if (this->method->Call(arguments...) == StatusError)
	      escape("unable to call the method");

	    break;
	  }
	case Routine::SchemeUnknown:
	  {
	    escape("unknown scheme");
	  }
	}

      leave();
    }

  }
}

#endif
