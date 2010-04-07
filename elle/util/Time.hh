//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/util/Time.hh
//
// created       julien quintard   [fri aug 21 23:56:01 2009]
// updated       julien quintard   [tue apr  6 19:32:31 2010]
//

#ifndef ELLE_UTIL_TIME_HH
#define ELLE_UTIL_TIME_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Boolean.hh>
#include <elle/core/Natural.hh>
#include <elle/core/Entity.hh>

#include <elle/io/Dumpable.hh>

#include <elle/archive/Archivable.hh>
#include <elle/archive/Archive.hh>

#include <elle/idiom/Close.hh>
# include <ctime>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace io;
  using namespace archive;

  namespace util
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a time stamp.
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

      Status		Convert(::time_t&) const;

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

      Natural8		dst;
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

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Report.hh>
#include <elle/misc/Maid.hh>

#endif
