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
// updated       julien quintard   [wed mar 17 13:52:32 2010]
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
#include <elle/misc/Callback.hh>

#include <elle/idiom/Open.hh>

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
      // classes
      //

      ///
      /// XXX
      ///
      class Shell:
	public Dumpable
      {
      public:
	virtual Status	Call(T&...) = 0;
      };

      ///
      /// XXX
      ///
      template <typename U>
      class Wrap:
	public Shell
      {
      public:
	//
	// types
	//
	typedef Status		(U::*Handler)(T&...);

	//
	// constructors & destructors
	//
	Wrap(U*,
	     Handler);

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
	U*		object;
	Handler		handler;
      };

      //
      // constructors & destructors
      //
      Method():
	Callback::Callback(Callback::TypeMethod)
      {
	enter();

	alert("this method should never have been called");
      }

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

      // entity
      embed(Entity, Method);
      // XXX

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Shell*		shell;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/misc/Method.hxx>

#endif
