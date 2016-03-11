#include <reactor/mutex.hh>

#include <elle/assert.hh>

namespace reactor
{
  Mutex::Mutex()
    : _locked(false)
  {}

  Mutex::~Mutex()
  {}

  bool
  Mutex::locked() const
  {
    return _locked;
  }

  bool
  Mutex::release()
  {
    ELLE_ASSERT(_locked);
    if (!_signal_one())
    {
      _locked = false;
      return false;
    }
    else
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
      _locked = true;
      return false;
    }
  }
}
