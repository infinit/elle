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
    /// Special mutex used by thread-safe version of ELLE_ATTRIBUTE.
    class rw_mutex;

    /// write_mutex, to protect writing.
    ///
    /// write_mutex can only be created by rw_mutex, which inherit from both
    /// write_mutex and read_mutex.
    class write_mutex
    {
    public:
      bool
      try_lock();
      void
      lock();
      void
      unlock();

    private:
      write_mutex();

    private:
      bool
      _try_lock(std::unique_lock<std::mutex>& lock);
      void
      _lock(std::unique_lock<std::mutex>& lock);
      void
      _set_locked();

    private:
      friend rw_mutex;
    };

    /// read_mutex, to protect reading.
    ///
    /// read_mutex can only be created by rw_mutex, which inherit from both
    /// read_mutex and write_mutex.
    class read_mutex
    {
    public:
      bool
      try_lock();
      void
      lock();
      void
      unlock();

    private:
      read_mutex();

    private:
      bool
      _try_lock(std::unique_lock<std::mutex>& lock);
      void
      _lock(std::unique_lock<std::mutex>& lock);
      void
      _set_locked();

    private:
      friend rw_mutex;
    };

    /// rw_mutex, to protect or reading or writing.
    ///
    /// rw_mutex makes sure a variable is protected from writes while being
    /// read or written.
    class rw_mutex
      : public write_mutex
      , public read_mutex
    {
    public:
      rw_mutex();

    private:
      friend class write_mutex;
      friend class read_mutex;
      ELLE_ATTRIBUTE(std::mutex, mutex);
      ELLE_ATTRIBUTE(std::condition_variable, condition);
      ELLE_ATTRIBUTE(int, readers);
      ELLE_ATTRIBUTE(bool, writing);
    };
  }
}

# include <elle/threading/rw-mutex.hxx>

#endif
