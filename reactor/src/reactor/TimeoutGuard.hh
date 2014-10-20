#ifndef REACTOR_TIMEOUT_GUARD_HH
# define REACTOR_TIMEOUT_GUARD_HH

# include <boost/asio/deadline_timer.hpp>

# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <reactor/duration.hh>

namespace reactor
{
  class TimeoutGuard
    : public elle::Printable
  {
  public:
    TimeoutGuard(reactor::Duration delay);
    ~TimeoutGuard();

    ELLE_ATTRIBUTE_R(reactor::Duration, delay);

  protected:
    void
    print(std::ostream& output) const override;

  private:
    ELLE_ATTRIBUTE(boost::asio::deadline_timer, timer);
  };
}

#endif
