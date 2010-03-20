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
// updated       julien quintard   [sat mar 20 03:30:09 2010]
//

#ifndef ELLE_MISC_CALLBACK_HXX
#define ELLE_MISC_CALLBACK_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Report.hh>
#include <elle/misc/Maid.hh>
#include <elle/misc/Function.hh>
#include <elle/misc/Method.hh>

#include <elle/idiom/Open.hh>

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

      escape("invalid callback type");
    }

  }
}

#endif
