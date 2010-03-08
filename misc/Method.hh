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
// updated       julien quintard   [tue mar  2 14:53:15 2010]
//

#ifndef ELLE_MISC_METHOD_HH
#define ELLE_MISC_METHOD_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/misc/Status.hh>
#include <elle/misc/Report.hh>
#include <elle/misc/Maid.hh>
#include <elle/misc/Callable.hh>
#include <elle/misc/Callback.hh>

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
      Callable*		object;
      Handler		handler;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/misc/Method.hxx>

#endif
