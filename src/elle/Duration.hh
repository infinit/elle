#pragma once

#include <boost/optional.hpp>

#include <elle/compiler.hh>
#include <elle/printf.hh> // Actually needed by our <chrono>.
#include <elle/time.hh>

namespace elle
{
  using namespace std::literals;

  /// A clock.
  using Clock = std::chrono::high_resolution_clock;
  /// A reference date.
  using Time = Clock::time_point;
  /// A duration related to our clock.
  using Duration = Clock::duration;

  /// An optional Duration.
  ///
  /// This is basically just `using DurationOpt = boost::optional<Duration>`.
  /// However then we don't have enough implicit conversions, and
  /// signatures such as `auto(DurationOpt d = 30s) -> void` fail
  /// to compile, we need `auto(DurationOpt d = DurationOpt{30s}) -> void`.
  /// This `struct` is here only to add a convenience ctor.
  struct DurationOpt
    : boost::optional<Duration>
  {
    using Super = boost::optional<Duration>;
    using Super::Super;

    template <typename Rep, typename Ratio>
    constexpr DurationOpt(std::chrono::duration<Rep, Ratio> d)
    noexcept(noexcept(std::chrono::duration_cast<Duration>(d)))
      : Super{std::chrono::duration_cast<Duration>(d)}
    {}

    // Needed although there is already a default ctor in Super. Why?
    // Cannot be constexpr with Boost 1.60.
    DurationOpt() noexcept
      : Super{}
    {}
  };

  /// The number of seconds in `d`.
  inline
  auto
  num_seconds(Duration d)
  {
    return std::chrono::duration_cast<std::chrono::seconds>(d).count();
  }

  /// The number of microseconds in `d`.
  inline
  auto
  num_milliseconds(Duration d)
  {
    return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
  }

  /// The number of microseconds in `d`.
  inline
  auto
  num_microseconds(Duration d)
  {
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
  }

  ELLE_API
  std::ostream&
  operator << (std::ostream& s, const DurationOpt& opt);
}
