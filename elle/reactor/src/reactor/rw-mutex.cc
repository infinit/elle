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
    , _locked(0)
    , _locked_recursive(0)
  {}

  bool
  RWMutex::WriteMutex::release()
  {
    if (_locked_recursive)
      {
        ELLE_LOG_TRACE("%s: release one of the %s recursive writing lock",
                       *this, this->_locked_recursive);
        --this->_locked_recursive;
        return false;
      }
    ELLE_LOG_TRACE_SCOPE("%s: release writing lock", *this);
    assert(_locked);
    if (!_signal_one())
      {
        _locked = 0;
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
    if (_locked == thread)
      {
        ++_locked_recursive;
        ELLE_LOG_TRACE_SCOPE("%s: already locked for writing by this"
                             " thread %s times.", *this, _locked_recursive);
        return false;
      }
    const bool reading = _owner._readers > 0;
    const bool writing = _locked;
    if (reading || writing)
    {
      if (reading)
        ELLE_LOG_TRACE("%s: already locked for reading, waiting.", *this);
      else
        ELLE_LOG_TRACE("%s: already locked for writing, waiting.", *this);
      // FIXME: we might be woken up to lock the mutex, but another
      // thread locks it before we can actually procced. Hence the
      // while. Not exactly sure this is exact and the best way to
      // proceed, though.
      bool res;
      assert(_locked);
      while (_locked)
        res = Waitable::_wait(thread);
      _locked = thread;
      return res;
    }
    else
    {
      ELLE_LOG_TRACE("%s: mutex is free, locking", *this);
      _locked = thread;
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
        if (_write._locked == thread)
          {
            ++_write._locked_recursive;
            ELLE_LOG_TRACE("%s: already locked for writing by this thread"
                           " %s times", *this, _write._locked_recursive);
            return false;
          }
        else
          ELLE_LOG_TRACE("%s: already locked for writing, waiting", *this)
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
    if (_write._locked)
      {
        ELLE_LOG_TRACE("%s: release one of the %s recursive writing lock",
                       *this, this->_write._locked_recursive);
        assert(this->_write._locked_recursive > 0);
        --this->_write._locked_recursive;
        return false;
      }

    ELLE_LOG_TRACE_SCOPE("%s: release one reading lock (readers now: %s)",
                         _readers - 1, *this);
    assert(_readers > 0);
    --_readers;
    if (!_readers)
      if (_write._signal_one())
        return true;
    return false;
  }

  RWMutex::WriteMutex&
  RWMutex::write()
  {
    return _write;
  }
}
