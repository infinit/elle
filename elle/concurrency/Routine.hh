//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Routine.hh
//
// created       julien quintard   [wed mar 24 19:50:08 2010]
// updated       julien quintard   [fri may 28 12:15:41 2010]
//

#ifndef ELLE_CONCURRENCY_ROUTINE_HH
#define ELLE_CONCURRENCY_ROUTINE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

namespace elle
{
  using namespace core;
  using namespace radix;

  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a routine, being a function, a method
    /// or static method.
    ///
    class Routine:
      public Object
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

      // object
      declare(Routine);

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
