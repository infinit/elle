#pragma once

#include <chrono>

namespace elle
{
  namespace chrono
  {
    template <typename Ratio, typename R = long>
    std::chrono::duration<R, Ratio>
    duration_parse(std::string const& repr);

    void
    duration_parse(std::string const& repr,
                   std::int64_t& ticks,
                   std::int64_t& num,
                   std::int64_t& denom);

  }
}

#include <elle/chrono.hxx>
