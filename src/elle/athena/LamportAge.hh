#pragma once

#include <boost/operators.hpp>

#include <elle/attribute.hh>
#include <elle/das/Symbol.hh>
#include <elle/das/printer.hh>
#include <elle/serialization/binary.hh>
#include <elle/serialization/json.hh>

namespace elle
{
  namespace athena
  {
    namespace symbol
    {
      ELLE_DAS_SYMBOL(timestamp);
    }

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
    /// exchange durations.
    class LamportAge
      : private boost::equivalent<LamportAge>
      , private boost::totally_ordered<LamportAge>
    {
    public:
      using Self = LamportAge;
      /// A reference date as an int.
      using Timestamp = int64_t;
      constexpr static auto null_timestamp = Timestamp{-1};

      /// A clock.
      using Clock = std::chrono::high_resolution_clock;
      /// A reference date.
      using Time = std::chrono::time_point<Clock>;

      constexpr LamportAge(Timestamp t) noexcept
        : _timestamp{t}
      {}

      constexpr LamportAge() noexcept
        : Self{null_timestamp}
      {}

      /// The default value LamportAge.
      constexpr static LamportAge null()
      {
        return {};
      }

      /// Set the reference time.
      void start(Time t = Clock::now())
      {
        _timestamp = timestamp(t);
      }

      /// From a date to an int.
      static Timestamp
      timestamp(Time t)
      {
        return std::chrono::duration_cast<std::chrono::milliseconds>
          (t.time_since_epoch()).count();
      }

      using serialization_tag = elle::serialization_tag;
      void
      serialize(elle::serialization::Serializer& ser)
      {
        if (ser.out())
        {
          auto& s = static_cast<elle::serialization::SerializerOut&>(ser);
          // Send the age, not the timestamp.
          s.serialize("age", timestamp(Clock::now()) - timestamp());
        }
        else
        {
          auto& s = static_cast<elle::serialization::SerializerIn&>(ser);
          // Receive the age, not the timestamp.
          _timestamp = timestamp(Clock::now()) - s.deserialize<Timestamp>("age");
        }
      }

      /// Default model, e.g. for printer.
      using Model = elle::das::Model<
        LamportAge,
        decltype(elle::meta::list(symbol::timestamp))>;
      ELLE_ATTRIBUTE_R(Timestamp, timestamp);
    };

    inline
    bool
    operator<(LamportAge const& lhs, LamportAge const& rhs)
    {
      return lhs.timestamp() < rhs.timestamp();
    }

    using elle::das::operator<<;
  }
}
