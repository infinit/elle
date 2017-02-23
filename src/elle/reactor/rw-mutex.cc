#include <elle/assert.hh>
#include <elle/log.hh>
#include <elle/reactor/rw-mutex.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/reactor/thread.hh>

ELLE_LOG_COMPONENT("elle.reactor.RWMutex");

namespace elle
{
  namespace reactor
  {

    /*-------.
    | Signal |
    `-------*/
    Thread*
    RWMutex::WriteMutex::_signal_one()
    {
      Thread* thread = Waitable::_signal_one();
      this->_locked = thread;
      return thread;
    }

    /*-----------.
    | WriteMutex |
    `-----------*/

    RWMutex::WriteMutex::WriteMutex(RWMutex& owner)
      : _owner(owner)
      , _locked(0)
      , _locked_recursive(0)
    {}

    bool
    RWMutex::WriteMutex::locked() const
    {
      return (this->_locked != nullptr);
    }

    bool
    RWMutex::WriteMutex::acquire()
    {
      auto thread = Scheduler::scheduler()->current();
      ELLE_TRACE_SCOPE("%s: lock for writing by %s", *this, *thread);
      if (_locked == thread)
      {
        ++_locked_recursive;
        ELLE_TRACE_SCOPE("%s: already locked for writing by this"
                         " thread %s times.", *this, _locked_recursive);
        return true;
      }
      const bool reading = _owner._readers > 0;
      const bool writing = _locked;
      if (reading || writing)
      {
        if (reading)
          ELLE_TRACE("%s: already locked for reading, waiting.", *this);
        else
        {
          ELLE_TRACE("%s: already locked for writing, waiting.", *this);
          ELLE_ASSERT(_locked);
        }
        return false;
      }
      else
      {
        ELLE_TRACE("%s: mutex is free, locking", *this);
        _locked = thread;
        return true;
      }
    }

    bool
    RWMutex::WriteMutex::release()
    {
      if (_locked_recursive)
        {
          ELLE_TRACE("%s: release one of the %s recursive writing lock",
                         *this, this->_locked_recursive);
          --this->_locked_recursive;
          return false;
        }
      ELLE_TRACE_SCOPE("%s: release writing lock", *this);
      ELLE_ASSERT(_locked);
      _signal();
      _owner._signal();
      _locked = 0;
      return false;
    }

    bool
    RWMutex::WriteMutex::_wait(Thread* thread, Waker const& waker)
    {
      ELLE_TRACE_SCOPE("%s: lock for writing by %s", *this, *thread);
      if (_locked == thread)
      {
        ELLE_TRACE_SCOPE("%s: already locked for writing by this"
                         " thread %s times.", *this, _locked_recursive);
        return false;
      }
      const bool reading = _owner._readers > 0;
      const bool writing = _locked;
      if (reading || writing)
      {
        if (reading)
          ELLE_TRACE("%s: already locked for reading, waiting.", *this);
        else
        {
          ELLE_TRACE("%s: already locked for writing, waiting.", *this);
          ELLE_ASSERT(_locked);
        }
        bool res = Waitable::_wait(thread, waker);
        return res;
      }
      else
      {
        ELLE_TRACE("%s: mutex is free", *this);
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
    RWMutex::_wait(Thread* thread, Waker const& waker)
    {
      ELLE_TRACE_SCOPE("%s: lock for reading by %s", *this, *thread);
      if (this->_write._locked)
        {
          if (_write._locked == thread)
            {
              ELLE_TRACE("%s: already locked for writing by this thread"
                             " %s times", *this, _write._locked_recursive);
              return false;
            }
          else
            ELLE_TRACE("%s: already locked for writing, waiting", *this)
              return Waitable::_wait(thread, waker);
        }
      else
      {
        ELLE_TRACE("%s: mutex is free, locking (readers now: %s)",
                       *this, _readers);
        return false;
      }
    }

    bool
    RWMutex::locked() const
    {
      return (this->_readers != 0);
    }

    bool
    RWMutex::acquire()
    {
      auto thread = Scheduler::scheduler()->current();
      ELLE_TRACE_SCOPE("%s: lock for reading by %s", *this, *thread);
      if (this->_write._locked)
      {
        if (_write._locked == thread)
        {
          ++_write._locked_recursive;
          ELLE_TRACE("%s: already locked for writing by this thread"
            " %s times", *this, _write._locked_recursive);
          return true;
        }
        else
          ELLE_TRACE("%s: already locked for writing, waiting", *this)
            return false;
      }
      else
      {
        ++_readers;
        ELLE_TRACE("%s: mutex is free, locking (readers now: %s)",
                       *this, _readers);
        return true;
      }
      elle::unreachable();
    }

    bool
    RWMutex::release()
    {
      if (_write._locked)
        {
          ELLE_TRACE("%s: release one of the %s recursive writing lock",
                         *this, this->_write._locked_recursive);
          ELLE_ASSERT_GT(this->_write._locked_recursive, 0);
          --this->_write._locked_recursive;
          return false;
        }

      ELLE_TRACE_SCOPE("%s: release one reading lock (readers now: %s)",
                           _readers - 1, *this);
      ELLE_ASSERT_GT(_readers, 0);
      --_readers;
      if (!_readers)
        _write._signal();
      return false;
    }

    RWMutex::WriteMutex&
    RWMutex::write()
    {
      return _write;
    }
  }
}
