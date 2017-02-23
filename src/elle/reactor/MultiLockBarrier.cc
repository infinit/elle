#include <elle/reactor/MultiLockBarrier.hh>
#include <elle/assert.hh>
#include <elle/log.hh>

ELLE_LOG_COMPONENT("elle.reactor.MultiLockBarrier");

namespace elle
{
  namespace reactor
  {
    /*-------------.
    | Construction |
    `-------------*/

    MultiLockBarrier::MultiLockBarrier(const std::string& name):
      Super(name),
      _locks(0)
    {}

    MultiLockBarrier::~MultiLockBarrier()
    {
      this->_assert_destructible();
    }

    /*---------.
    | Openness |
    `---------*/

    MultiLockBarrier::operator bool() const
    {
      return this->opened();
    }

    bool
    MultiLockBarrier::opened() const
    {
      return this->locks() == 0;
    }

    MultiLockBarrier::Lock::Lock(MultiLockBarrier& barrier)
      : _barrier(barrier)
    {
      ELLE_DEBUG_SCOPE("%s: creation", *this);
      this->_barrier._lock();
    }

    MultiLockBarrier::Lock::Lock(Lock const& lock)
      : Lock(lock._barrier)
    {
      ELLE_DEBUG("%s has been copied from %s", *this, lock);
    }

    MultiLockBarrier::Lock::~Lock()
    {
      ELLE_DEBUG_SCOPE("%s: destuction", *this);
      this->_barrier._unlock();
    }

    MultiLockBarrier::Lock
    MultiLockBarrier::lock()
    {
      return Lock(*this);
    }

    void
    MultiLockBarrier::_lock()
    {
      ELLE_DEBUG_SCOPE("%s: add a lock", *this);
      ++this->_locks;
      ELLE_DEBUG("it has %s lock(s) now", this->_locks);
      this->_changed(this->_locks);
    }

    void
    MultiLockBarrier::_unlock()
    {
      ELLE_ASSERT_GT(this->_locks, 0u);
      ELLE_DEBUG_SCOPE("%s: remove a lock", *this);
      --this->_locks;
      ELLE_DEBUG("it has %s lock(s) now", this->_locks);
      this->_changed(this->_locks);
      if (this->opened())
        this->_signal();
    }

    /*---------.
    | Waitable |
    `---------*/

    bool
    MultiLockBarrier::_wait(Thread* thread, Waker const& waker)
    {
      if (this->opened())
        return false;
      else
        return Super::_wait(thread, waker);
    }
  }
}
