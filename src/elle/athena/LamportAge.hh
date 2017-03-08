#pragma once

#include <boost/operators.hpp>

#include <elle/attribute.hh>
#include <elle/serialization/binary.hh>
#include <elle/serialization/json.hh>
#include <elle/printf.hh>

namespace elle
{
  namespace athena
  {
    /// An age (duration since a date to now) made to be transported.
    ///
    /// Clocks are relative and we should not expect a timestamp on
    /// one machine to make sense on another.  However, sometimes, we
    /// need some form of distributed time-out.  Suppose for instance
    /// we need to register the fact that a machine in a cluster
    /// disappeared for too long: we want to inform other participants
    /// in the cluster for this age.  But we must not send the initial
    /// timestamp, rather the age of the disaparition.  However, the
    /// duration is of little importance: what we actually want to
    /// record locally is the time when the machine disapeared, but
    /// propagate to the others the corresponding age.
    ///
    /// In other words, locally we keep the disappearance time, but we
    /// exchange ages.
    class LamportAge
      : private boost::equivalent<LamportAge>
      , private boost::totally_ordered<LamportAge>
      , public Printable
    {
    public:
      using Self = LamportAge;
      /// A reference date as an int.
      using Timestamp = int64_t;

      /// A clock.
      using Clock = std::chrono::high_resolution_clock;
      /// A reference date.
      using Time = std::chrono::time_point<Clock>;
      /// A duration.
      using Duration = std::chrono::seconds;

      constexpr LamportAge(Time t) noexcept
        : _timestamp{t}
      {}

      /// Initialized with the Clock's epoch.
      constexpr LamportAge() = default;

      /// The default value LamportAge.
      constexpr static LamportAge null()
      {
        return {};
      }

      /// Set the reference time.
      void start(Time t = Clock::now())
      {
        _timestamp = t;
      }

      /// Get the current age.
      Duration
      age() const
      {
        return std::chrono::duration_cast<Duration>(Clock::now() - _timestamp);
      }

      /// From a date to a number of milliseconds as an int.
      static Timestamp
      timestamp(Time t)
      {
        return std::chrono::duration_cast<Duration>
          (t.time_since_epoch()).count();
      }

      /*-----------.
      | Printable  |
      `-----------*/
    public:
      /// Print pretty representation to \a stream.
      void
      print(std::ostream& s) const override
      {
        elle::fprintf(s, "LamportAge(t=%s, a=%s)",
                      std::chrono::duration_cast<std::chrono::seconds>
                      (timestamp().time_since_epoch()),
                      age());
      }

      /*--------------.
      | Serializable  |
      `--------------*/
      using serialization_tag = elle::serialization_tag;
      void
      serialize(elle::serialization::Serializer& ser)
      {
        if (ser.out())
        {
          auto& s = static_cast<elle::serialization::SerializerOut&>(ser);
          // Send the age, not the timestamp.
          s.serialize("age", age());
        }
        else
        {
          auto& s = static_cast<elle::serialization::SerializerIn&>(ser);
          // Receive the age, not the timestamp.
          _timestamp = Clock::now() - s.deserialize<Duration>("age");
        }
      }
      ELLE_ATTRIBUTE_R(Time, timestamp);
    };

    inline
    bool
    operator<(LamportAge const& lhs, LamportAge const& rhs)
    {
      // Don't be too precise when comparing the timestamps.
      return LamportAge::timestamp(lhs.timestamp())
        < LamportAge::timestamp(rhs.timestamp());
    }
  }
}
