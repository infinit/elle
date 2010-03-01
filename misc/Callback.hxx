//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       misc
//
// file          /home/mycure/infinit/elle/misc/Callback.hxx
//
// created       julien quintard   [thu feb  4 22:08:43 2010]
// updated       julien quintard   [sun feb 28 12:49:23 2010]
//

#include <elle/misc/Function.hh>
#include <elle/misc/Method.hh>

#ifndef ELLE_MISC_CALLBACK_HXX
#define ELLE_MISC_CALLBACK_HXX

//
// ---------- includes --------------------------------------------------------
//

namespace elle
{
  namespace misc
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method calls the callback implementation specific
    /// Call() method.
    ///
    template <typename... T>
    Status		Callback::Trigger(T&...			parameters)
    {
      enter();

      switch (this->type)
	{
	case Callback::TypeFunction:
	  {
	    Function<T...>*	function =
	      static_cast<Function<T...>*>(this);

	    return (function->Call(parameters...));
	  }
	case Callback::TypeMethod:
	  {
	    Method<T...>*		method =
	      static_cast<Method<T...>*>(this);

	    return (method->Call(parameters...));
	  }
	}

      leave();
    }

  }
}

#endif
