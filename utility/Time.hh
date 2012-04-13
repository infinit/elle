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

#include <elle/types.hh>
#include <elle/types.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

#include <elle/package/Archive.hh>

#include <elle/utility/Duration.hh>

#include <elle/system/Platform.hh>

#include <elle/idiom/Close.hh>
# include <QDateTime>
# include <ctime>

# if defined(INFINIT_WINDOWS)
#  include <windows.h>
# endif

#include <elle/idiom/Open.hh>

namespace elle
{

  using namespace radix;
  using namespace package;

  namespace utility
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a timestamp through the number of nanoseconds
    /// since epoch.
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
      Status            Current();

      Status            Get(::time_t&) const;
      Status            Set(const ::time_t&);

      Status            Get(::QDateTime&) const;
      Status            Set(const ::QDateTime&);

#if defined(INFINIT_WINDOWS)
      Status            Get(::FILETIME&) const;
      Status            Set(const ::FILETIME&);
#endif

      //
      // interfaces
      //

      // object
      declare(Time);
      Boolean           operator==(const Time&) const;
      Boolean           operator<(const Time&) const;
      Boolean           operator>(const Time&) const;

      Time              operator+(const Time&);
      Time              operator-(const Time&);

      Time              operator+(const Duration&);
      Time              operator-(const Duration&);

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      // archivable
      Status            Serialize(Archive&) const;
      Status            Extract(Archive&);

      //
      // attributes
      //
      Natural64         nanoseconds;
    };

  }
}

#endif
