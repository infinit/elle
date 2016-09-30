#ifndef ELLE_UTILITY_TIME_HH
# define ELLE_UTILITY_TIME_HH

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/Printable.hh>

# include <elle/utility/fwd.hh>

namespace elle
{
  namespace utility
  {

    ///
    /// this class represents a timestamp through the number of nanoseconds
    /// since epoch.
    ///
    class Time:
      public Printable
    {
      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Return the current time.
      static
      Time
      current();

      //
      // constructors & destructors
      //
    public:
      Time();
      virtual
      ~Time();

      //
      // methods
      //
      Status            Current();

      //
      // interfaces
      //

      ELLE_OPERATOR_ASSIGNMENT(Time); // XXX

      Boolean           operator==(const Time&) const;
      Boolean           operator<(const Time&) const;
      Boolean           operator>(const Time&) const;

      Time              operator+(const Time&);
      Time              operator-(const Time&);

      Time              operator+(const Duration&);
      Time              operator-(const Duration&);

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      // printable
      virtual
      void
      print(std::ostream& stream) const;

      //
      // attributes
      //
      Natural64         nanoseconds;

      //
      // helpers.
      //
      Natural64         microseconds();
      Natural64         milliseconds();
      Natural64         seconds();
    };

  }
}

#include <elle/utility/Time.hxx>

#endif
