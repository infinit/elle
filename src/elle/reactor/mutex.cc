#include <elle/reactor/mutex.hh>
#include <elle/assert.hh>

namespace elle
{
  namespace reactor
  {
    Mutex::Mutex()
      : _locked(false)
    {}

    Mutex::~Mutex()
    = default;

    bool
    Mutex::locked() const
    {
      return _locked;
    }

    bool
    Mutex::release()
    {
      ELLE_ASSERT(_locked);
      _locked = false;
      _signal();
      return false;
    }

    bool
    Mutex::acquire()
    {
      if (_locked)
        return false;
      _locked = true;
      return true;
    }

    bool
    Mutex::_wait(Thread* thread, Waker const& waker)
    {
      if (_locked)
      {
        this->Waitable::_wait(thread, waker);
        _locked = true;
        return true;
      }
      else
      {
        return false;
      }
    }
  }
}
