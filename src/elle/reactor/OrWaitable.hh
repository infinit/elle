#pragma once

#include <elle/reactor/Waitable.hh>

namespace elle
{
  namespace reactor
  {
    class OrWaitable
      : public Waitable
    {
    public:
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

    reactor::OrWaitable
    operator || (reactor::Waitable& lhs, reactor::Waitable& rhs);
  }
}
