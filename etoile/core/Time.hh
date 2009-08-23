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
// updated       julien quintard   [sat aug 22 00:41:48 2009]
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
      Time&		operator=(const Time&);
      Boolean		operator==(const Time&) const;
      Boolean		operator!=(const Time&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Natural8		seconds;
      Natural8		minutes;
      Natural8		hours;

      Natural8		day;
      Natural8		month;
      Natural16		year;
    };

  }
}

#endif
