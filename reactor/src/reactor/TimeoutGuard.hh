#ifndef REACTOR_TIMEOUT_GUARD_HH
# define REACTOR_TIMEOUT_GUARD_HH

# include <boost/asio/deadline_timer.hpp>

# include <elle/attribute.hh>

# include <reactor/duration.hh>

namespace reactor
{
  class TimeoutGuard
  {
  public:
    TimeoutGuard(reactor::Duration delay);
    ~TimeoutGuard();

    ELLE_ATTRIBUTE_R(reactor::Duration, delay);
    ELLE_ATTRIBUTE(boost::asio::deadline_timer, timer);
  };
}

#endif
