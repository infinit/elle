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
      bool
      try_lock();
      void
      lock();
      void
      unlock();

    protected:
      write_mutex();

    private:
      bool
      _try_lock(std::unique_lock<std::mutex>& lock);
      void
      _lock(std::unique_lock<std::mutex>& lock);
      void
      _set_locked();
    };

    class read_mutex
    {
    public:
      bool
      try_lock();
      void
      lock();
      void
      unlock();

    protected:
      read_mutex();

    private:
      bool
      _try_lock(std::unique_lock<std::mutex>& lock);
      void
      _lock(std::unique_lock<std::mutex>& lock);
      void
      _set_locked();
    };

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
