//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/utility/Duration.hh
//
// created       julien quintard   [thu aug 11 10:40:35 2011]
// updated       julien quintard   [thu aug 11 10:51:14 2011]
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
    /// XXX
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

	  UnitMilliseconds,
	  UnitSeconds,
	  UnitMinutes,
	  UnitHours,
	  UnitDays,
	  UnitMonths,
	  UnitYears
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
