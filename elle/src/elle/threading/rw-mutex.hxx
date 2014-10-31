#ifndef ELLE_THREADING_RW_MUTEX_HXX
# define ELLE_THREADING_RW_MUTEX_HXX

namespace elle
{
  namespace threading
  {
    inline
    write_mutex::write_mutex()
    {}

    inline
    bool
    write_mutex::try_lock()
    {
      ELLE_TRACE_SCOPE("%s: try lock write", *static_cast<rw_mutex*>(this));
      std::unique_lock<std::mutex> lock(static_cast<rw_mutex*>(this)->_mutex);
      return this->_try_lock(lock);
    }

    inline
    void
    write_mutex::lock()
    {
      ELLE_TRACE_SCOPE("%s: lock write", *static_cast<rw_mutex*>(this));
      std::unique_lock<std::mutex> lock(static_cast<rw_mutex*>(this)->_mutex);
      this->_lock(lock);
    }

    inline
    void
    write_mutex::unlock()
    {
      ELLE_TRACE_SCOPE("%s: unlock write", *static_cast<rw_mutex*>(this));
      std::unique_lock<std::mutex> lock(static_cast<rw_mutex*>(this)->_mutex);
      ELLE_ASSERT(static_cast<rw_mutex*>(this)->_writing);
      ELLE_ASSERT(static_cast<rw_mutex*>(this)->_readers == 0);
      static_cast<rw_mutex*>(this)->_writing = false;
      ELLE_TRACE("%s: release internal lock", *static_cast<rw_mutex*>(this));
      static_cast<rw_mutex*>(this)->_condition.notify_one();
    }

    inline
    bool
    write_mutex::_try_lock(std::unique_lock<std::mutex>& lock)
    {
      if (static_cast<rw_mutex*>(this)->_readers > 0 || static_cast<rw_mutex*>(this)->_writing)
        return false;
      this->_set_locked();
      return true;
    }

    inline
    void
    write_mutex::_lock(std::unique_lock<std::mutex>& lock)
    {
      while (!this->_try_lock(lock))
      {
        ELLE_TRACE("%s: wait for internal lock", *static_cast<rw_mutex*>(this));
        static_cast<rw_mutex*>(this)->_condition.wait(lock);
      }
    }

    inline
    void
    write_mutex::_set_locked()
    {
      ELLE_TRACE("%s: acquire internal lock for writing", *static_cast<rw_mutex*>(this));
      static_cast<rw_mutex*>(this)->_writing = true;
    }

    inline
    bool
    read_mutex::try_lock()
    {
      ELLE_TRACE_SCOPE("%s: try lock read", *static_cast<rw_mutex*>(this));
      std::unique_lock<std::mutex> lock(static_cast<rw_mutex*>(this)->_mutex);
      return this->_try_lock(lock);
    }

    inline
    void
    read_mutex::lock()
    {
      ELLE_TRACE_SCOPE("%s: lock read", *static_cast<rw_mutex*>(this));
      std::unique_lock<std::mutex> lock(static_cast<rw_mutex*>(this)->_mutex);
      this->_lock(lock);
    }

    inline
    void
    read_mutex::unlock()
    {
      ELLE_TRACE_SCOPE("%s: unlock read", *static_cast<rw_mutex*>(this));
      std::unique_lock<std::mutex> lock(static_cast<rw_mutex*>(this)->_mutex);
      ELLE_ASSERT(!static_cast<rw_mutex*>(this)->_writing);
      if (--static_cast<rw_mutex*>(this)->_readers == 0)
      {
        ELLE_TRACE("%s: release internal lock", *static_cast<rw_mutex*>(this));
        static_cast<rw_mutex*>(this)->_condition.notify_one();
      }
    }

    inline
    read_mutex::read_mutex()
    {}

    inline
    bool
    read_mutex::_try_lock(std::unique_lock<std::mutex>& lock)
    {
      if (static_cast<rw_mutex*>(this)->_writing)
        return false;
      this->_set_locked();
      return true;
    }

    inline
    void
    read_mutex::_lock(std::unique_lock<std::mutex>& lock)
    {
      while (!this->_try_lock(lock))
      {
        ELLE_TRACE("%s: wait for internal lock", *static_cast<rw_mutex*>(this));
        static_cast<rw_mutex*>(this)->_condition.wait(lock);
      }
    }

    inline
    void
    read_mutex::_set_locked()
    {
      ELLE_TRACE("%s: acquire internal lock for reading", *static_cast<rw_mutex*>(this));
      ++static_cast<rw_mutex*>(this)->_readers;
    }

    inline
    rw_mutex::rw_mutex()
      : _mutex()
      , _condition()
      , _readers(0)
      , _writing(false)
    {}
  }
}

#endif
