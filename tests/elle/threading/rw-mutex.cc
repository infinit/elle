#include <thread>

#include <elle/log.hh>
#include <elle/test.hh>
#include <elle/threading/rw-mutex.hh>

// Check two threads can read concurrently.
static
void
multiple_read()
{
  bool locked1 = false;
  std::condition_variable locked1_barrier;
  bool locked2 = false;
  std::condition_variable locked2_barrier;
  std::mutex locked_mutex;
  elle::threading::rw_mutex mtx;
  auto reader =
    [&] (bool& locked_mine,
         std::condition_variable& locked_mine_barrier,
         bool& locked_his,
         std::condition_variable& locked_his_barrier)
    {
      std::unique_lock<elle::threading::read_mutex> rw_lock(mtx);
      std::unique_lock<std::mutex> lock(locked_mutex);
      locked_mine = true;
      locked_mine_barrier.notify_all();
      if (!locked_his)
        locked_his_barrier.wait(lock);
    };
  std::thread reader1(
    [&]
    {
      reader(locked1, locked1_barrier, locked2, locked2_barrier);
    });
  std::thread reader2(
    [&]
    {
      reader(locked2, locked2_barrier, locked1, locked1_barrier);
    });
  reader1.join();
  reader2.join();
}

template <typename Mutex>
static
void
acquire(elle::threading::rw_mutex& mutex,
        std::mutex& sync_mutex,
        bool& sync,
        std::condition_variable& sync_barrier,
        bool& wait,
        std::condition_variable& wait_barrier)
{
  ELLE_LOG_COMPONENT("elle.threading.rw_mutex");
  std::unique_lock<std::mutex> lock(sync_mutex);
  std::unique_lock<Mutex> rw_lock(mutex);
  ELLE_LOG("acquire: locked");
  sync = true;
  sync_barrier.notify_all();
  ELLE_LOG("acquire: wait for failure");
  wait_barrier.wait(lock);
  rw_lock.unlock();
  ELLE_LOG("acquire: unlocked");
}

template <typename Mutex>
static
void
block(elle::threading::rw_mutex& mutex,
      std::mutex& sync_mutex,
      bool& sync,
      std::condition_variable& sync_barrier,
      bool& wait,
      std::condition_variable& wait_barrier)
{
  std::unique_lock<std::mutex> lock(sync_mutex);
  if (!wait)
  {
    ELLE_LOG("block: wait until locked");
    wait_barrier.wait(lock);
  }
  std::unique_lock<Mutex> rw_lock
    (mutex, std::defer_lock);
  ELLE_LOG("block: fail lock")
  BOOST_CHECK(!rw_lock.try_lock());
  sync = true;
  sync_barrier.notify_all();
  lock.unlock();
  ELLE_LOG("block: lock")
  rw_lock.lock();
};

// Check two threads can't write concurrently.
template <typename Acquired, typename Blocked>
static
void
blocks()
{
  elle::threading::rw_mutex mutex;
  std::mutex sync_mutex;
  bool reading = false;
  std::condition_variable reading_barrier;
  bool writing = false;
  std::condition_variable writing_barrier;
  std::thread acquire(
    [&]
    {
      try
      {
        ELLE_LOG_COMPONENT("elle.threading.rw_mutex");
        std::unique_lock<std::mutex> lock(sync_mutex);
        std::unique_lock<Acquired> rw_lock(mutex);
        ELLE_LOG("acquire: locked");
        reading = true;
        reading_barrier.notify_all();
        ELLE_LOG("acquire: wait for failure");
        writing_barrier.wait(lock);
        rw_lock.unlock();
        ELLE_LOG("acquire: unlocked");
      }
      catch (...)
      {
        BOOST_ERROR(elle::sprintf("acquire: %s", elle::exception_string()));
      }
    });
  std::thread block(
    [&]
    {
      try
      {
        std::unique_lock<std::mutex> lock(sync_mutex);
        if (!reading)
        {
          ELLE_LOG("block: wait until locked");
          reading_barrier.wait(lock);
        }
        std::unique_lock<Blocked> rw_lock
          (mutex, std::defer_lock);
        ELLE_LOG("block: fail lock")
          if (rw_lock.try_lock())
          {
            BOOST_ERROR("lock was acquired");
            rw_lock.unlock();
          }
        writing = true;
        writing_barrier.notify_all();
        lock.unlock();
        ELLE_LOG("block: lock")
          rw_lock.lock();
      }
      catch (...)
      {
        BOOST_ERROR(elle::sprintf("block: %s", elle::exception_string()));
      }
    });
  acquire.join();
  block.join();
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(multiple_read), 0, 3);
  auto read_blocks_read =
    &blocks<elle::threading::read_mutex, elle::threading::read_mutex>;
  suite.add(BOOST_TEST_CASE(read_blocks_read), 1, 3);
  auto read_blocks_write =
    &blocks<elle::threading::read_mutex, elle::threading::write_mutex>;
  suite.add(BOOST_TEST_CASE(read_blocks_write), 0, 3);
  auto write_blocks_read =
    &blocks<elle::threading::write_mutex, elle::threading::read_mutex>;
  suite.add(BOOST_TEST_CASE(write_blocks_read), 0, 3);
  auto write_blocks_write =
    &blocks<elle::threading::write_mutex, elle::threading::write_mutex>;
  suite.add(BOOST_TEST_CASE(write_blocks_write), 0, 3);
}
