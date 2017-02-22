#include <elle/reactor/OrWaitable.hh>
#include <elle/printf.hh>

namespace elle
{
  namespace reactor
  {
    OrWaitable::OrWaitable(Waitable& lhs, Waitable& rhs)
      : _lhs(lhs)
      , _rhs(rhs)
    {}

    void
    OrWaitable::print(std::ostream& output) const
    {
      elle::fprintf(output, "%s || %s", this->_lhs, this->_rhs);
    }

    bool
    OrWaitable::_wait(Thread* t, Waker const& waker)
    {
      if (!this->_lhs._wait(t, [this] (Thread* t)
                            {
                              this->_rhs._unwait(t);
                              this->_signal_one(t);
                            }))
        return false;
      if (!this->_rhs._wait(t, [this] (Thread* t)
                            {
                              this->_lhs._unwait(t);
                              this->_signal_one(t);
                            }))
      {
        this->_lhs._unwait(t);
        return false;
      }
      return Waitable::_wait(t, waker);
    }

    reactor::OrWaitable
    operator || (reactor::Waitable& lhs, reactor::Waitable& rhs)
    {
      return OrWaitable(lhs, rhs);
    }
  }
}
