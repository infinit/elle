//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Time.hh
//
// created       julien quintard   [fri aug 21 23:56:01 2009]
// updated       julien quintard   [sat mar 20 04:11:25 2010]
//

#ifndef ETOILE_CORE_TIME_HH
#define ETOILE_CORE_TIME_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <ctime>

namespace etoile
{
  namespace core
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Time:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // constructors & destructors
      //
      Time();

      //
      // methods
      //
      Status		Current();

      //
      // interfaces
      //

      // entity
      declare(Entity, Time);
      Boolean		operator==(const Time&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Natural8		second;
      Natural8		minute;
      Natural8		hour;

      Natural8		day;
      Natural8		month;
      Natural16		year;
    };

//
// ---------- operators -------------------------------------------------------
//

    Time		operator+(const Time&,
				  const Time&);

    Boolean		operator<(const Time&,
				  const Time&);

  }
}

#endif
