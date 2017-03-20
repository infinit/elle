#pragma once

#include <chrono>

namespace elle
{
  namespace chrono
  {
    /// Parse \a Duration.
    ///
    /// XXX: Partially duplicated with elle/src/chrono.
    template <typename Ratio, typename R = long>
    std::chrono::duration<R, Ratio>
    duration_parse(std::string const& repr);

    /// Parse \a Duration.
    ///
    /// XXX: Partially duplicated with elle/src/chrono.
    void
    duration_parse(std::string const& repr,
                   std::int64_t& ticks,
                   std::int64_t& num,
                   std::int64_t& denom);
  }
}

#include <elle/chrono.hxx>
