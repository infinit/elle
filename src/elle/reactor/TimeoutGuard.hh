#pragma once

#include <boost/asio/deadline_timer.hpp>

#include <elle/Printable.hh>
#include <elle/attribute.hh>
#include <elle/reactor/duration.hh>

namespace elle
{
  namespace reactor
  {
    /// Guard a scope by throwing a TimeoutException if not destroyed within
    /// a given time.
    ///
    /// @code{.cc}
    ///
    /// {
    ///   TimeoutGuard g(200_ms);
    ///   // do_read and do_write have 200 ms to run, otherwise the TimeoutGuard
    ///   // will raise an exception.
    ///   do_read():
    ///   do_write();
    /// }
    ///
    /// @endcode
    class TimeoutGuard
      : public elle::Printable
    {
    public:
      /// Construct a TimeoutGuard with the given delay.
      ///
      /// @param delay The Duration before the TimeoutGuard throws an Exception.
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
