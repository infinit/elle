//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/misc/Method.hh
//
// created       julien quintard   [thu feb  4 23:03:30 2010]
// updated       julien quintard   [fri feb  5 13:20:09 2010]
//

//
// ---------- includes --------------------------------------------------------
//

///
/// this include has been put here to avoid pre-processing issues.
///
#include <elle/misc/Callback.hh>

#ifndef ELLE_MISC_METHOD_HH
#define ELLE_MISC_METHOD_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

#include <elle/misc/Callable.hh>

namespace elle
{
  namespace misc
  {

    ///
    /// this class represents a method callback.
    ///
    template <typename... T>
    class Method:
      public Callback
    {
    public:
      //
      // types
      //
      typedef Status		(Callable::*Handler)(T&...);

      //
      // constructors & destructors
      //
      template <typename X>
      Method(X*,
	     Status (X::*)(T&...));

      //
      // methods
      //
      Status		Call(T&...);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Callable*		instance;
      Handler		handler;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/misc/Method.hxx>

#endif
