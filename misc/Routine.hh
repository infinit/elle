//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/misc/Routine.hh
//
// created       julien quintard   [wed mar 24 19:50:08 2010]
// updated       julien quintard   [thu mar 25 00:39:56 2010]
//

#ifndef ELLE_MISC_ROUTINE_HH
#define ELLE_MISC_ROUTINE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Entity.hh>

#include <elle/misc/Status.hh>

#include <elle/io/Dumpable.hh>

namespace elle
{
  using namespace core;
  using namespace io;

  namespace misc
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a routine, being a function, a method
    /// or static method.
    ///
    class Routine:
      public Entity,
      public Dumpable
    {
    public:
      //
      // enumerations
      //
      enum Type
	{
	  TypeUnknown,

	  TypeFunction,
	  TypeMethod
	};

      //
      // constructors & destructors
      //
      Routine();
      Routine(const Type);

      //
      // methods
      //
      template <typename... T>
      Status		Trigger(T*...);
      template <typename... T>
      Status		Call(T&...);

      //
      // interfaces
      //

      // entity
      declare(Entity, Routine);
      // XXX

      // dumpable
      Status		Dump(const Natural32 = 0) const;

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

#include <elle/misc/Routine.hxx>

#endif
