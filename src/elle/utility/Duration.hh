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
               const uint64_t);
      Duration(Duration const& other) = default;

      //
      // interfaces
      //

      ELLE_OPERATOR_NO_ASSIGNMENT(Duration);

      bool operator==(const Duration&) const;

      // dumpable
      Status Dump(const uint32_t = 0) const;

      //
      // attributes
      //
      Unit              unit;
      uint64_t         value;
    };

  }
}

#include <elle/utility/Duration.hxx>
