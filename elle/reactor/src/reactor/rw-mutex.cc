#include <elle/log.hh>

#include <reactor/rw-mutex.hh>
#include <reactor/thread.hh>

ELLE_LOG_TRACE_COMPONENT("reactor.RWMutex");

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
    ELLE_LOG_TRACE_SCOPE("%s: release writing lock", *this);
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
    ELLE_LOG_TRACE_SCOPE("%s: lock for writing by %s", *this, *thread);
    const bool reading = _owner._readers > 0;
    const bool writing = _locked;
    if (reading || _locked)
    {
      if (reading)
        ELLE_LOG_TRACE("%s: already locked for reading, waiting.", *this);
      else
        ELLE_LOG_TRACE("%s: already locked for writing, waiting.", *this);
      return Waitable::_wait(thread);
    }
    else
    {
      ELLE_LOG_TRACE("%s: mutex is free, locking", *this);
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
    ELLE_LOG_TRACE_SCOPE("%s: lock for reading by %s", *this, *thread);
    if (_write._locked)
      {
        ELLE_LOG_TRACE_SCOPE("%s: already locked for writing, waiting",
                             *this, *thread);
        return Waitable::_wait(thread);
      }
    else
    {
      ++_readers;
      ELLE_LOG_TRACE("%s: mutex is free, locking (readers now: %s)",
                     *this, _readers);
      return false;
    }
  }

  bool
  RWMutex::release()
  {
    ELLE_LOG_TRACE_SCOPE("%s: release one reading lock (readers now: %s)",
                         _readers - 1, *this);
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
