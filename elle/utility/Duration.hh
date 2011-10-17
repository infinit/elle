//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu aug 11 10:40:35 2011]
//

#ifndef ELLE_UTILITY_DURATION_HH
#define ELLE_UTILITY_DURATION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Object.hh>

#include <elle/package/Archive.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace package;

  namespace utility
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class defines durations of time which can then be used
    /// with the Time class to go forward and backward in time for instance.
    ///
    class Duration:
      public Object
    {
    public:
      //
      // enumerations
      //
      enum Unit
	{
	  UnitUnknown,

	  UnitNanoseconds,
	  UnitMicroseconds,
	  UnitMilliseconds,
	  UnitSeconds,
	  UnitMinutes
	};

      //
      // constructors & destructors
      //
      Duration();
      Duration(const Unit,
	       const Natural64);

      //
      // interfaces
      //

      // object
      declare(Duration);
      Boolean		operator==(const Duration&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Unit		unit;
      Natural64		value;
    };

  }
}

#endif
