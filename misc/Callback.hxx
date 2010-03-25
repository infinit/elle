//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/misc/Callback.hxx
//
// created       julien quintard   [wed mar 24 23:43:50 2010]
// updated       julien quintard   [thu mar 25 18:13:15 2010]
//

#ifndef ELLE_MISC_CALLBACK_HXX
#define ELLE_MISC_CALLBACK_HXX

namespace elle
{
  namespace misc
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    template <typename... T>
    Callback<T...>::Callback():
      Routine::Routine(Routine::TypeCallback),

      scheme(Routine::SchemeUnknown)
    {
    }

    ///
    /// function-based constructor.
    ///
    template <typename... T>
    Callback<T...>::Callback(typename Function<T&...>::Handler	handler):
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
    Callback<T...>::Callback(typename
			       Method<T&...>::
			         template Wrap<C>::Handler	handler,
			     C*					object):
      Routine::Routine(Routine::TypeCallback),

      scheme(Routine::SchemeMethod),
      method(new Method<T&...>(handler, object))
    {
    }

    ///
    /// copy constructor.
    ///
    template <typename... T>
    Callback<T...>::Callback(const Callback&			callback):
      scheme(callback.scheme)
    {
      enter();

      // copy the content depending on the scheme.
      switch (this->scheme)
	{
	case Routine::SchemeFunction:
	  {
	    // clone the function.
	    if (callback.function->Clone((Entity*&)this->function) ==
		StatusError)
	      alert("unable to clone the function");

	    break;
	  }
	case Routine::SchemeMethod:
	  {
	    // clone the method.
	    if (callback.method->Clone((Entity*&)this->method) ==
		StatusError)
	      alert("unable to clone the method");

	    break;
	  }
	}

      release();
    }

    ///
    /// destructor.
    ///
    template <typename... T>
    Callback<T...>::~Callback()
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
	}
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Callback<T...>, template <typename... T>);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the callback.
    ///
    template <typename... T>
    Status		Callback<T...>::Dump(const Natural32	margin) const
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
	}

      leave();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method calls the callback implementation specific Call() method.
    ///
    template <typename... T>
    Status		Callback<T...>::Call(T&...		arguments)
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
	default:
	  {
	    escape("invalid callback scheme");
	  }
	}

      leave();
    }

  }
}

#endif
