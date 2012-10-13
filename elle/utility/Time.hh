#ifndef ELLE_UTILITY_TIME_HH
# define ELLE_UTILITY_TIME_HH

# include <elle/types.hh>

# include <elle/radix/Object.hh>

# include <elle/utility/fwd.hh>

# include <elle/idiom/Close.hh>
#  include <chrono>
#  include <ctime>
#  if defined(INFINIT_WINDOWS)
#   include <windows.h>
#  endif
# include <elle/idiom/Open.hh>

namespace elle
{
  namespace utility
  {

    ///
    /// this class represents a timestamp through the number of nanoseconds
    /// since epoch.
    ///
    class Time:
      public radix::Object
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

      //
      // attributes
      //
      Natural64         nanoseconds;
    };

  }
}

#include <elle/utility/Time.hxx>

#endif
