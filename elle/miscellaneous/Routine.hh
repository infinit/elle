//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/miscellaneous/Routine.hh
//
// created       julien quintard   [wed mar 24 19:50:08 2010]
// updated       julien quintard   [mon mar 29 23:13:01 2010]
//

#ifndef ELLE_MISCELLANEOUS_ROUTINE_HH
#define ELLE_MISCELLANEOUS_ROUTINE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Entity.hh>

#include <elle/miscellaneous/Status.hh>

#include <elle/io/Dumpable.hh>

namespace elle
{
  using namespace core;
  using namespace io;

  namespace miscellaneous
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

	  TypeCallback,
	  TypeEntrance
	};

      enum Scheme
	{
	  SchemeUnknown,

	  SchemeFunction,
	  SchemeMethod
	};

      //
      // constructors & destructors
      //
      Routine();
      Routine(const Type);

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

#endif
