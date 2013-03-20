#include <reactor/mutex.hh>

namespace reactor
{
  Mutex::Mutex()
    : _locked(false)
  {}

  bool
  Mutex::locked() const
  {
    return _locked;
  }

  bool
  Mutex::release()
  {
    assert(_locked);
    if (!_signal_one())
    {
      _locked = false;
      return false;
    }
    else
      return true;
  }

  bool
  Mutex::_wait(Thread* thread)
  {
    if (_locked)
    {
      Waitable::_wait(thread);
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
