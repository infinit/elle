#pragma once

#include <boost/asio/deadline_timer.hpp>

#include <elle/Printable.hh>
#include <elle/attribute.hh>
#include <elle/reactor/duration.hh>

namespace elle
{
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
}
