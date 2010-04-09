//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/miscellaneous/Entrance.hxx
//
// created       julien quintard   [wed mar 24 23:43:50 2010]
// updated       julien quintard   [tue mar 30 17:29:18 2010]
//

#ifndef ELLE_MISCELLANEOUS_ENTRANCE_HXX
#define ELLE_MISCELLANEOUS_ENTRANCE_HXX

namespace elle
{
  namespace miscellaneous
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    template <typename... T>
    Entrance<T...>::Entrance():
      Routine::Routine(Routine::TypeEntrance),

      scheme(Routine::SchemeUnknown)
    {
    }

    ///
    /// function-based constructor.
    ///
    template <typename... T>
    Entrance<T...>::Entrance(typename Function<T&...>::Handler	handler):
      Routine::Routine(Routine::TypeEntrance),

      scheme(Routine::SchemeFunction),
      function(new Function<T&...>(handler))
    {
    }

    ///
    /// method-based constructor.
    ///
    template <typename... T>
    template <typename C>
    Entrance<T...>::Entrance(typename
			       Method<T&...>::
			         template Wrap<C>::Handler	handler,
			     C*					object):
      Routine::Routine(Routine::TypeEntrance),

      scheme(Routine::SchemeMethod),
      method(new Method<T&...>(handler, object))
    {
    }

    ///
    /// copy constructor.
    ///
    template <typename... T>
    Entrance<T...>::Entrance(const Entrance&			entrance):
      scheme(entrance.scheme)
    {
      enter();

      // copy the content depending on the scheme.
      switch (this->scheme)
	{
	case Routine::SchemeFunction:
	  {
	    // clone the function.
	    if (entrance.function->Clone((Entity*&)this->function) ==
		StatusError)
	      alert("unable to clone the function");

	    break;
	  }
	case Routine::SchemeMethod:
	  {
	    // clone the method.
	    if (entrance.method->Clone((Entity*&)this->method) ==
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
    Entrance<T...>::~Entrance()
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
    embed(Entity, Entrance<T...>, template <typename... T>);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the entrance.
    ///
    template <typename... T>
    Status		Entrance<T...>::Dump(const Natural32	margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Entrance]" << std::endl;

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
    /// this method calls the entrance implementation by displaying the
    /// report should an error occur.
    ///
    template <typename... T>
    Status		Entrance<T...>::Trigger(T&...		arguments)
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
	      show();

	    break;
	  }
	case Routine::SchemeMethod:
	  {
	    // call the method.
	    if (this->method->Call(arguments...) == StatusError)
	      show();

	    break;
	  }
	default:
	  {
	    escape("invalid entrance scheme");
	  }
	}

      leave();
    }

  }
}

#endif
