#ifndef ELLE_THREADING_RW_MUTEX_HH
# define ELLE_THREADING_RW_MUTEX_HH

# include <condition_variable>
# include <mutex>

# include <elle/assert.hh>
# include <elle/attribute.hh>
# include <elle/log.hh>

ELLE_LOG_COMPONENT("elle.threading.rw_mutex");

namespace elle
{
  namespace threading
  {
    class write_mutex
    {
    public:
      write_mutex()
        : _mutex()
        , _condition()
        , _readers(0)
        , _writing(false)
      {}

      bool
      try_lock()
      {
        ELLE_TRACE_SCOPE("%s: try lock write", *this);
        std::unique_lock<std::mutex> lock(this->_mutex);
        return this->_try_lock(lock);
      }

      void
      lock()
      {
        ELLE_TRACE_SCOPE("%s: lock write", *this);
        std::unique_lock<std::mutex> lock(this->_mutex);
        this->_lock(lock);
      }

      void
      unlock()
      {
        ELLE_TRACE_SCOPE("%s: unlock write", *this);
        std::unique_lock<std::mutex> lock(this->_mutex);
        ELLE_ASSERT(this->_writing);
        ELLE_ASSERT(this->_readers == 0);
        this->_writing = false;
        ELLE_TRACE("%s: release internal lock", *this);
        this->_condition.notify_one();
      }

    private:
      bool
      _try_lock(std::unique_lock<std::mutex>& lock)
      {
        if (this->_readers > 0 || this->_writing)
          return false;
        this->_set_locked();
        return true;
      }

      void
      _lock(std::unique_lock<std::mutex>& lock)
      {
        if (!this->_try_lock(lock))
        {
          ELLE_TRACE("%s: wait for internal lock", *this);
          this->_condition.wait(lock);
          ELLE_ASSERT(!this->_writing);
          ELLE_ASSERT(this->_readers == 0);
          this->_set_locked();
        }
      }

      void
      _set_locked()
      {
        ELLE_TRACE("%s: acquire internal lock for writing", *this);
        this->_writing = true;
      }

      ELLE_ATTRIBUTE(std::mutex, mutex);
      ELLE_ATTRIBUTE(std::condition_variable, condition);
      ELLE_ATTRIBUTE(int, readers);
      ELLE_ATTRIBUTE(bool, writing);
      friend class rw_mutex;
    };

    class rw_mutex:
      public write_mutex
    {
    public:
      rw_mutex()
        : write_mutex()
      {}

      bool
      try_lock()
      {
        ELLE_TRACE_SCOPE("%s: try lock read", *this);
        std::unique_lock<std::mutex> lock(this->_mutex);
        return this->_try_lock(lock);
      }

      void
      lock()
      {
        ELLE_TRACE_SCOPE("%s: lock read", *this);
        std::unique_lock<std::mutex> lock(this->_mutex);
        this->_lock(lock);
      }

      void
      unlock()
      {
        ELLE_TRACE_SCOPE("%s: unlock read", *this);
        std::unique_lock<std::mutex> lock(this->_mutex);
        ELLE_ASSERT(!this->_writing);
        if (--this->_readers == 0)
        {
          ELLE_TRACE("%s: release internal lock", *this);
          this->_condition.notify_one();
        }
      }

    private:
      bool
      _try_lock(std::unique_lock<std::mutex>& lock)
      {
        if (this->_writing)
          return false;
        this->_set_locked();
        return true;
      }

      void
      _lock(std::unique_lock<std::mutex>& lock)
      {
        if (!this->_try_lock(lock))
        {
          ELLE_TRACE("%s: wait for internal lock", *this);
          this->_condition.wait(lock);
          ELLE_ASSERT(!this->_writing);
          this->_set_locked();
        }
      }

      void
      _set_locked()
      {
        ELLE_TRACE("%s: acquire internal lock for reading", *this);
        ++this->_readers;
      }
    };
  }
}

#endif
