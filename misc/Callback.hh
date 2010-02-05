//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/misc/Callback.hh
//
// created       julien quintard   [thu feb  4 16:59:50 2010]
// updated       julien quintard   [fri feb  5 00:47:24 2010]
//

#ifndef ELLE_MISC_CALLBACK_HH
#define ELLE_MISC_CALLBACK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

#include <elle/misc/Callable.hh>

namespace elle
{
  namespace misc
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a callback in its abstract form, being
    /// either a method or a function callback.
    ///
    class Callback
    {
    public:
      //
      // enumerations
      //
      enum Type
	{
	  TypeFunction,
	  TypeMethod
	};

      //
      // constructors & destructors
      //
      Callback(const Type);

      //
      // methods
      //
      template <typename... T>
      Status		Trigger(T&...);

      //
      // attributes
      //
      Type		type;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/misc/Callback.hxx>

#endif
