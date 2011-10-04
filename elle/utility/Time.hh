//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri aug 21 23:56:01 2009]
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

#include <elle/utility/Duration.hh>

#include <elle/idiom/Close.hh>
# include <QDateTime>
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
    /// this class represents a timestamp.
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

      Status		Get(::time_t&) const;
      Status		Get(::QDateTime&) const;

      Status		Set(const ::time_t&);
      Status		Set(const ::QDateTime&);

      //
      // interfaces
      //

      // object
      declare(Time);
      Boolean		operator==(const Time&) const;
      Boolean		operator<(const Time&) const;
      Boolean		operator>(const Time&) const;

      Time		operator+(const Time&);
      Time		operator-(const Time&);

      Time		operator+(const Duration&);
      Time		operator-(const Duration&);

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Natural16		millisecond;
      Natural8		second;
      Natural8		minute;
      Natural8		hour;

      Natural8		day;
      Natural8		month;
      Natural16		year;
    };

  }
}

#endif
