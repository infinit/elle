//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/utility/Time.hh
//
// created       julien quintard   [fri aug 21 23:56:01 2009]
// updated       julien quintard   [fri jul  8 16:21:47 2011]
//

#ifndef ELLE_UTILITY_TIME_HH
#define ELLE_UTILITY_TIME_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Boolean.hh>
#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

#include <elle/package/Archive.hh>

#include <elle/idiom/Close.hh>
# include <ctime>
#include <elle/idiom/Open.hh>

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
    /// this class represents a time stamp.
    ///
    class Time:
      public Object
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

      // object
      declare(Time);
      Boolean		operator==(const Time&) const;
      Boolean		operator<(const Time&) const;
      Time		operator+(const Time&);

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

  }
}

#endif
