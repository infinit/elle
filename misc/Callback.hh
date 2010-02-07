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
// updated       julien quintard   [sat feb  6 22:29:46 2010]
//

#ifndef ELLE_MISC_CALLBACK_HH
#define ELLE_MISC_CALLBACK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/io/IO.hh>

#include <elle/misc/Callable.hh>

namespace elle
{
  using namespace io;

  namespace misc
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a callback in its abstract form, being
    /// either a method or a function callback.
    ///
    /// callbacks are useful even if a event system such as QT is already
    /// used. indeed QT's system is static meaning that the prototype of
    /// signals must be known at the time the developer writes the code
    /// while, by using callbacks, the code is dynamic and type-aware through
    /// the use of templates and can therefore adapt to different cases.
    ///
    /// finally, callbacks can be either functions, static methods or
    /// methods while QT's slots can only be methods.
    ///
    class Callback:
      public Dumpable
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
