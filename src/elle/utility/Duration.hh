#pragma once

#include <elle/types.hh>
#include <elle/operator.hh>

#include <utility>
ELLE_OPERATOR_RELATIONALS();

namespace elle
{
  namespace utility
  {

    ///
    /// this class defines durations of time which can then be used
    /// with the Time class to go forward and backward in time for instance.
    ///
    class Duration
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
      Duration(Duration const& other) = default;

      //
      // interfaces
      //

      ELLE_OPERATOR_NO_ASSIGNMENT(Duration);

      bool operator==(const Duration&) const;

      // dumpable
      Status Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Unit              unit;
      Natural64         value;
    };

  }
}

#include <elle/utility/Duration.hxx>
