#include <reactor/rw-mutex.hh>
#include <reactor/thread.hh>

namespace reactor
{
  /*-----------.
  | WriteMutex |
  `-----------*/

  RWMutex::WriteMutex::WriteMutex(RWMutex& owner)
    : _owner(owner)
    , _locked(false)
  {}

  bool
  RWMutex::WriteMutex::release()
  {
    assert(_locked);
    if (!_signal_one())
    {
      _locked = false;
      int woken = _owner._signal();
      _owner._readers += woken;
      return woken > 0;
    }
    else
      return true;
  }

  bool
  RWMutex::WriteMutex::_wait(Thread* thread)
  {
    if (_owner._readers > 0 || _locked)
    {
      return Waitable::_wait(thread);
    }
    else
    {
      _locked = true;
      return false;
    }
  }

  /*--------.
  | RWMutex |
  `--------*/

  RWMutex::RWMutex()
    : _write(*this)
    , _readers(0)
  {}

  bool
  RWMutex::_wait(Thread* thread)
  {
    if (_write._locked)
      // FIXME
      return Waitable::_wait(thread);
    else
    {
      ++_readers;
      return false;
    }
  }

  bool
  RWMutex::release()
  {
    assert(_readers > 0);
    --_readers;
    if (!_readers)
      if (_write._signal_one())
      {
        _write._locked = true;
        return true;
      }
    return false;
  }

  RWMutex::WriteMutex&
  RWMutex::write()
  {
    return _write;
  }
}
