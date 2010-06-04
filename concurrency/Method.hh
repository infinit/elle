//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Method.hh
//
// created       julien quintard   [thu feb  4 23:03:30 2010]
// updated       julien quintard   [fri may 28 12:15:30 2010]
//

#ifndef ELLE_CONCURRENCY_METHOD_HH
#define ELLE_CONCURRENCY_METHOD_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>
#include <elle/radix/Entity.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;

  namespace concurrency
  {

    ///
    /// this class represents a method.
    ///
    template <typename... T>
    class Method:
      public Object
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
	public Entity
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

      // object
      declare(Method<T...>);

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

#include <elle/concurrency/Method.hxx>

#endif
