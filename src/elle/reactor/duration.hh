#pragma once

#include <elle/Duration.hh>
#include <elle/reactor/asio.hh>

namespace elle
{
  namespace reactor
  {
    /// A clock.
    using elle::Clock;
    /// A reference date.
    using elle::Time;
    /// The type of our timers.
    using WTimer = boost::asio::basic_waitable_timer<Clock>;

    using elle::Duration;
    using elle::DurationOpt;
  }
}
