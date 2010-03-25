//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/misc/Routine.hxx
//
// created       julien quintard   [wed mar 24 20:56:33 2010]
// updated       julien quintard   [thu mar 25 00:25:10 2010]
//

#ifndef ELLE_MISC_ROUTINE_HXX
#define ELLE_MISC_ROUTINE_HXX

//
// ---------- includes --------------------------------------------------------
//
#include <elle/misc/Function.hh>
#include <elle/misc/Method.hh>

namespace elle
{
  namespace misc
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method calls the entrance implementation since the given arguments
    /// are pointers.
    ///
    template <typename... T>
    Status		Routine::Trigger(T*...			arguments)
    {
      enter();

      // depending on the type.
      switch (this->type)
	{
	case Routine::TypeFunction:
	  {
	    Function<T...>*	function = static_cast<Function<T...>*>(this);

	    // call the function.
	    return (function->Call(arguments...));
	  }
	case Routine::TypeMethod:
	  {
	    Method<T...>*	method = static_cast<Method<T...>*>(this);

	    // call the method.
	    return (method->Call(arguments...));
	  }
	}

      escape("invalid routine type");
    }

    ///
    /// this method calls the callback implementation since the given arguments
    /// are referenced.
    ///
    template <typename... T>
    Status		Routine::Call(T&...			arguments)
    {
      enter();

      // depending on the type.
      switch (this->type)
	{
	case Routine::TypeFunction:
	  {
	    Function<T...>*	function = static_cast<Function<T...>*>(this);

	    // call the function.
	    return (function->Call(arguments...));
	  }
	case Routine::TypeMethod:
	  {
	    Method<T...>*	method = static_cast<Method<T...>*>(this);

	    // call the method.
	    return (method->Call(arguments...));
	  }
	}

      escape("invalid routine type");
    }

  }
}

#endif
