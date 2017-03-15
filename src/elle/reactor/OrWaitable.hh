#pragma once

#include <elle/reactor/Waitable.hh>

namespace elle
{
  namespace reactor
  {
    /// OrWaitable allows for waiting on one Waitable or another.
    ///
    /// \code{.cc}
    ///
    /// elle::reactor::Semaphore s;
    /// elle::reactor::Barrier b;
    ///
    /// ...
    ///
    /// // Make the current Thread to wait for s or b to signal.
    /// (s || b).wait();
    ///
    /// \endcode.
    class OrWaitable
      : public Waitable
    {
    public:
      /// Construct an OrWaitable from two Waitables.
      ///
      /// \param lhs First Waitable.
      /// \param lhs Second Waitable.
      OrWaitable(Waitable& lhs, Waitable& rhs);
      virtual
      void
      print(std::ostream& output) const override;

    protected:
      virtual
      bool
      _wait(Thread* t, Waker const& waker) override;

    private:
      ELLE_ATTRIBUTE(Waitable&, lhs);
      ELLE_ATTRIBUTE(Waitable&, rhs);
    };

    /// Allow for using || (or) operator to create OrWaitables.
    reactor::OrWaitable
    operator || (reactor::Waitable& lhs, reactor::Waitable& rhs);
  }
}
