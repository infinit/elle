#pragma once

#include <chrono>

#include <elle/windows.h>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace elle
{
  // https://stackoverflow.com/a/4918873/1353549
  template <typename Clock, typename Duration>
  inline static
  boost::posix_time::ptime
  to_boost(const std::chrono::time_point<Clock, Duration>& from)
  {
    using duration_t = std::chrono::nanoseconds;
    using rep_t = duration_t::rep;
    rep_t d = std::chrono::duration_cast<duration_t>(from.time_since_epoch()).count();
    rep_t sec = d / 1000000000;
    rep_t nsec = d % 1000000000;
    return boost::posix_time::from_time_t(0)
      + boost::posix_time::seconds(static_cast<long>(sec))
#ifdef BOOST_DATE_TIME_HAS_NANOSECONDS
      + boost::posix_time::nanoseconds(nsec);
#else
      + boost::posix_time::microseconds((nsec+500)/1000);
#endif
  }

  template <typename Clock, typename Duration>
  inline static
  std::chrono::time_point<Clock, Duration>
  from_boost(const boost::posix_time::ptime& from)
  {
    boost::posix_time::time_duration const time_since_epoch
      = from - boost::posix_time::from_time_t(0);
    std::chrono::time_point<Clock, Duration> t
      = std::chrono::system_clock::from_time_t(time_since_epoch.total_seconds());
    long nsec
      = time_since_epoch.fractional_seconds()
      * (1000000000 / time_since_epoch.ticks_per_second());
    return t + std::chrono::nanoseconds(nsec);
  };
}
