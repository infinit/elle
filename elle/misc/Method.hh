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
// updated       julien quintard   [mon mar 29 21:50:58 2010]
//

#ifndef ELLE_MISC_METHOD_HH
#define ELLE_MISC_METHOD_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/io/Dumpable.hh>

#include <elle/misc/Status.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  namespace misc
  {

    ///
    /// this class represents a method.
    ///
    template <typename... T>
    class Method:
      public Entity,
      public Dumpable
    {
    public:
      //
      // classes
      //

      ///
      /// the Shell and Wrap classes are required so that the Method
      /// class can be retrieved from T..., meaning without C which is
      /// only used at the Method construction time.
      ///
      /// this class is a base for inheritance.
      ///
      class Shell:
	public Dumpable
      {
      public:
	//
	// methods
	//
	virtual Status Call(T...) = 0;
      };

      ///
      /// this class is C-specific.
      ///
      template <typename C>
      class Wrap:
	public Shell
      {
      public:
	//
	// types
	//
	typedef Status		(C::*Handler)(T...);

	//
	// constructors & destructors
	//
	Wrap(Handler,
	     C*);
	Wrap(const Wrap<C>&);

	//
	// methods
	//
	Status		Call(T...);

	//
	// interfaces
	//

	// entity
	declare(Entity, Wrap);
	// XXX

	// dumpable
	Status		Dump(const Natural32 = 0) const;

	//
	// attributes
	//
	C*		object;
	Handler		handler;
      };

      //
      // constructors & destructors.
      //
      template <typename C>
      Method(Status (C::*)(T...),
	     C*);

      //
      // methods
      //
      Status		Call(T...);

      //
      // interfaces
      //

      // entity
      declare(Entity, Method<T...>);
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
