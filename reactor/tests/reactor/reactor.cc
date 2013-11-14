#include "reactor.hh"

#include <elle/finally.hh>
#include <elle/test.hh>

#include <reactor/asio.hh>
#include <reactor/Barrier.hh>
#include <reactor/Scope.hh>
#include <reactor/duration.hh>
#include <reactor/exception.hh>
#include <reactor/mutex.hh>
#include <reactor/rw-mutex.hh>
#include <reactor/semaphore.hh>
#include <reactor/signal.hh>
#include <reactor/sleep.hh>
#include <reactor/storage.hh>
#include <reactor/thread.hh>

/*-----------------.
| Global shortcuts |
`-----------------*/

reactor::Scheduler* sched = 0;

Fixture::Fixture()
{
  sched = new reactor::Scheduler;
}

Fixture::~Fixture()
{
  delete sched;
  sched = 0;
}

static
void
yield()
{
  reactor::Scheduler::scheduler()->current()->yield();
}

static
bool
_wait(reactor::Waitable& s,
     reactor::DurationOpt timeout = reactor::DurationOpt())
{
  return reactor::Scheduler::scheduler()->current()->wait(s, timeout);
}

static
void
sleep(reactor::Duration d)
{
  return reactor::Scheduler::scheduler()->current()->sleep(d);
}

class BeaconException:
  public elle::Exception
{
public:
  BeaconException()
    : elle::Exception("beacon")
  {}
};

#ifdef INFINIT_WINDOWS
static
void
sleep(int sec)
{
  ::Sleep(sec);
}
static
void
usleep(int usec)
{
  ::Sleep(usec / 1000000.0);
}
#endif

/*-------.
| Basics |
`-------*/

static
void
coro(int& step)
{
  BOOST_CHECK_EQUAL(step, 0);
  ++step;
  reactor::yield();
  BOOST_CHECK_EQUAL(step, 1);
  ++step;
}

static
void
test_basics_one()
{

  reactor::Scheduler sched;

  int step = 0;
  reactor::Thread t(sched, "coro", std::bind(coro, std::ref(step)));
  sched.run();
  BOOST_CHECK_EQUAL(step, 2);
}

static
void
coro1(int& step)
{
  BOOST_CHECK(step == 0 || step == 1);
  ++step;
  yield();
  BOOST_CHECK(step == 2 || step == 3);
  ++step;
  yield();
  BOOST_CHECK(step == 4);
  ++step;
  yield();
  BOOST_CHECK(step == 5);
}

static
void
coro2(int& step)
{
  BOOST_CHECK(step == 0 || step == 1);
  ++step;
  yield();
  BOOST_CHECK(step == 2 || step == 3);
  ++step;
}

static
void
test_basics_interleave()
{
  Fixture f;

  int step = 0;
  reactor::Thread c1(*sched, "1", boost::bind(coro1, boost::ref(step)));
  reactor::Thread c2(*sched, "2", boost::bind(coro2, boost::ref(step)));
  sched->run();
  BOOST_CHECK_EQUAL(step, 5);
}

/*--------.
| Signals |
`--------*/

static
void
waiter(int& step,
       reactor::Waitables& waitables)
{
  BOOST_CHECK_EQUAL(step, 0);
  sched->current()->wait(waitables);
  ++step;
}

static
void
sender_one(int& step,
           reactor::Signal& s, int expect)
{
  BOOST_CHECK_EQUAL(step, 0);
  yield();
  BOOST_CHECK_EQUAL(step, 0);
  yield();
  BOOST_CHECK_EQUAL(step, 0);
  s.signal();
  BOOST_CHECK_EQUAL(step, 0);
  yield();
  yield();
  BOOST_CHECK_EQUAL(step, expect);
}

static
void
sender_two(int& step,
           reactor::Signal& s1,
           reactor::Signal& s2)
{
  BOOST_CHECK_EQUAL(step, 0);
  yield();
  BOOST_CHECK_EQUAL(step, 0);
  yield();
  BOOST_CHECK_EQUAL(step, 0);
  s1.signal();
  BOOST_CHECK_EQUAL(step, 0);
  yield();
  BOOST_CHECK_EQUAL(step, 0);
  yield();
  BOOST_CHECK_EQUAL(step, 0);
  yield();
  s2.signal();
  yield();
  yield();
  BOOST_CHECK_EQUAL(step, 1);
}

static
void
test_signals_one_on_one()
{
  Fixture f;

  int step = 0;
  reactor::Signal signal;
  reactor::Waitables signals;
  signals << signal;
  reactor::Thread w(*sched, "waiter",
                    boost::bind(waiter, boost::ref(step), signals));
  reactor::Thread s(*sched, "sender",
                    boost::bind(sender_one, boost::ref(step),
                                boost::ref(signal), 1));
  sched->run();
  BOOST_CHECK_EQUAL(step, 1);
}

static
void
test_signals_one_on_two()
{
  Fixture f;

  int step = 0;
  reactor::Signal signal1;
  reactor::Signal signal2;
  reactor::Waitables signals;
  signals << signal1 << signal2;
  reactor::Thread w(*sched, "waiter",
                    boost::bind(waiter, boost::ref(step), signals));
  reactor::Thread s(*sched, "sender",
                    boost::bind(sender_two, boost::ref(step),
                                boost::ref(signal1), boost::ref(signal2)));
  sched->run();
  BOOST_CHECK_EQUAL(step, 1);
}

static
void
test_signals_two_on_one()
{
  Fixture f;

  int step = 0;
  reactor::Signal signal;
  reactor::Waitables signals;
  signals << signal;
  reactor::Thread w1(*sched, "waiter1",
                     boost::bind(waiter, boost::ref(step), signals));
  reactor::Thread w2(*sched, "waiter2",
                     boost::bind(waiter, boost::ref(step), signals));
  reactor::Thread s(*sched, "sender",
                    boost::bind(sender_one, boost::ref(step),
                                boost::ref(signal), 2));
  sched->run();
  BOOST_CHECK_EQUAL(step, 2);
}

static
void
waiter_timeout()
{
  reactor::Signal s;
  bool finished = _wait(s, boost::posix_time::milliseconds(10));
  BOOST_CHECK(!finished);
  s.signal();
}

static
void
test_signals_timeout()
{
  Fixture f;

  reactor::Thread t(*sched, "waiter", waiter_timeout);
  sched->run();
}

/*--------.
| Barrier |
`--------*/

static
void
barrier_closed()
{
  reactor::Scheduler sched;
  reactor::Barrier barrier;
  bool beacon = false;
  reactor::Thread waiter(sched, "waiter", [&] {
      BOOST_CHECK(!barrier.opened());
      _wait(barrier);
      BOOST_CHECK(barrier.opened());
      BOOST_CHECK(!beacon);
      beacon = true;
    });
  reactor::Thread opener(sched, "opener", [&] {
      yield();
      yield();
      yield();
      BOOST_CHECK(!beacon);
      barrier.open();
    });
  sched.run();
  BOOST_CHECK(beacon);
}

static
void
barrier_opened()
{
  reactor::Scheduler sched;
  reactor::Barrier barrier;
  barrier.open();
  BOOST_CHECK(barrier.opened());
  reactor::Thread waiter(sched, "waiter", [&] {
      _wait(barrier);
    });
  sched.run();
}

/*------.
| Scope |
`------*/

namespace scope
{
  static
  void
  empty()
  {
    reactor::Scheduler sched;
    reactor::Thread t(
      sched, "main",
      []
      {
        elle::With<reactor::Scope> s;
      });
    sched.run();
  }

  static
  void
  wait()
  {
    reactor::Scheduler sched;
    reactor::Thread t(
      sched, "main",
      []
      {
        bool beacon1 = false;
        bool beacon2 = false;
        elle::With<reactor::Scope>() << [&] (reactor::Scope& s)
        {
          s.run_background(
            "1",
            [&]
            {
              reactor::yield();
              reactor::yield();
              beacon1 = true;
            });
          reactor::yield();
          s.run_background(
            "2",
            [&]
            {
              reactor::yield();
              reactor::yield();
              beacon2 = true;
            });
          s.wait();
          BOOST_CHECK(beacon1);
          BOOST_CHECK(beacon2);
        };
      });

    sched.run();
  }

  static
  void
  exception()
  {
    reactor::Scheduler sched;
    reactor::Thread t(
      sched, "main",
      []
      {
        bool beacon = false;
        elle::With<reactor::Scope>() << [&] (reactor::Scope& s)
        {
          s.run_background(
            "1",
            [&]
            {
              {
                try
                {
                  reactor::yield();
                  reactor::yield();
                }
                catch (...)
                {
                  beacon = true;
                  throw;
                }
              }
              BOOST_FAIL("should have been killed");
            });
          s.run_background(
            "2",
            []
            {
              throw BeaconException();
            });
          BOOST_CHECK_THROW(s.wait(), BeaconException);
          BOOST_CHECK(beacon);
        };
      });
    sched.run();
  }

  static
  void
  multiple_exception()
  {
    reactor::Scheduler sched;
    reactor::Thread t(
      sched, "main",
      []
      {
        bool beacon = false;
        elle::With<reactor::Scope>() << [&] (reactor::Scope& s)
        {
          auto thrower = [] { throw BeaconException(); };

          s.run_background(
            "0",
            [&]
            {
              {
                try
                {
                  reactor::yield();
                  reactor::yield();
                }
                catch (...)
                {
                  beacon = true;
                  throw;
                }
              }
              BOOST_FAIL("should have been killed");
            });

          for (int i = 1; i <= 2; ++i)
            s.run_background(elle::sprintf("%s", i), [&] { thrower(); });

          BOOST_CHECK_THROW(s.wait(), BeaconException);
          BOOST_CHECK(beacon);
        };
      });
    sched.run();
  }

  static
  void
  terminate()
  {
    reactor::Scheduler sched;
    reactor::Barrier ready;
    reactor::Thread t(
      sched, "main",
      [&]
      {
        elle::With<reactor::Scope>() << [&] (reactor::Scope& s)
        {
          s.run_background(
            "1",
            [&]
            {
              {
                try
                {
                  ready.open();
                  reactor::sleep(1_sec);
                  BOOST_FAIL("should have been killed");
                }
                catch (...)
                {
                  t.terminate();
                  reactor::sleep(1_sec);
                  throw;
                }
              }
            });
        };
      });
    sched.run();
  }

  static
  void
  terminate_all()
  {
    reactor::Scheduler sched;
    reactor::Barrier ready;
    reactor::Thread t(
      sched, "main",
      [&]
      {
        elle::With<reactor::Scope>() << [&] (reactor::Scope& s)
        {
          s.run_background(
            "1",
            [&]
            {
              while (true)
                reactor::yield();
            });

          s.run_background(
            "2",
            [&]
            {
              while (true)
                reactor::yield();
            });

          s.terminate_now();
        };
      });
    sched.run();
  }
}

/*------.
| Sleep |
`------*/

static
void
sleeper1(int& step)
{
  BOOST_CHECK(step == 0 || step == 1);
  ++step;
  sleep(boost::posix_time::milliseconds(400));
  BOOST_CHECK_EQUAL(step, 3);
  ++step;
}

static
void
sleeper2(int& step)
{
  BOOST_CHECK(step == 0 || step == 1);
  ++step;
  sleep(boost::posix_time::milliseconds(200));
  BOOST_CHECK_EQUAL(step, 2);
  ++step;
}

static
void
test_sleep_interleave()
{
  Fixture f;

  int step = 0;
  reactor::Thread s1(*sched, "sleeper1",
                     boost::bind(sleeper1, boost::ref(step)));
  reactor::Thread s2(*sched, "sleeper2",
                     boost::bind(sleeper2, boost::ref(step)));
  sched->run();
}

static
boost::posix_time::ptime
now()
{
  return boost::posix_time::microsec_clock::local_time();
}

static
void
sleep_timer(int& iterations)
{
  reactor::Duration delay(boost::posix_time::milliseconds(1000));

  while (--iterations)
  {
    boost::posix_time::ptime start(now());
    sleep(delay);
    double elapsed = (now() - start).total_milliseconds();
    double expected =  delay.total_milliseconds();
    BOOST_CHECK_CLOSE(elapsed, expected, double(15));
  }
}

static
void
test_sleep_timing()
{
  Fixture f;

  int iterations = 8;
  reactor::Thread t(*sched, "sleeper",
                    boost::bind(sleep_timer, boost::ref(iterations)));
  sched->run();
  BOOST_CHECK_EQUAL(iterations, 0);
}

/*-----.
| Join |
`-----*/

static
void
joined(int& count)
{
  BOOST_CHECK_EQUAL(count, 0);
  yield();
  ++count;
  yield();
  yield();
  ++count;
}

static
void
join_waiter(reactor::Thread& thread,
            int& count)
{
  wait(thread);
  BOOST_CHECK_EQUAL(count, 2);
  ++count;
}

static
void
test_join()
{
  Fixture f;

  int count = 0;
  reactor::Thread j(*sched, "joined",
                    boost::bind(joined, boost::ref(count)));
  reactor::Thread w(*sched, "waiter",
                    boost::bind(join_waiter,
                                boost::ref(j), boost::ref(count)));
  sched->run();
  BOOST_CHECK_EQUAL(count, 3);
}

static
void
join_waiter_multiple(reactor::Thread& thread,
                     int& count)
{
  yield();
  BOOST_CHECK(thread.state() == reactor::Thread::state::done);
  _wait(thread);
  _wait(thread);
  ++count;
}

static
void
test_join_multiple()
{
  Fixture f;

  int count = 0;
  reactor::Thread e(*sched, "empty", [] {});
  reactor::Thread w(*sched, "waiter",
                    boost::bind(join_waiter_multiple,
                                boost::ref(e), boost::ref(count)));
  sched->run();
  BOOST_CHECK_EQUAL(count, 1);
}

static
void
sleeping_beauty()
{
  sleep(boost::posix_time::milliseconds(200));
}

static
void
prince_charming(reactor::Thread& sleeping_beauty)
{
  bool finished = _wait(sleeping_beauty, boost::posix_time::milliseconds(100));
  BOOST_CHECK(!finished);
  BOOST_CHECK(!sleeping_beauty.done());
  finished = _wait(sleeping_beauty, boost::posix_time::milliseconds(200));
  BOOST_CHECK(finished);
  BOOST_CHECK(sleeping_beauty.done());
}

static
void
test_join_timeout()
{
  Fixture f;

  reactor::Thread sb(*sched, "sleeping beauty", sleeping_beauty);
  reactor::Thread pc(*sched, "prince charming",
                     boost::bind(prince_charming, boost::ref(sb)));
  sched->run();
}

/*--------.
| Timeout |
`--------*/

static
void
timeout(reactor::Signal& s,
        bool expect)
{
  bool finished = _wait(s, boost::posix_time::milliseconds(500));
  BOOST_CHECK(finished == expect);
  BOOST_CHECK(s.waiters().empty());
}

static
void
timeout_send(reactor::Signal& s)
{
  yield();
  BOOST_CHECK_EQUAL(s.waiters().size(), 1);
  s.signal();
}

static
void
test_timeout_do()
{
  Fixture f;

  reactor::Signal s;
  reactor::Thread t(*sched, "timeout",
                    boost::bind(timeout, boost::ref(s), false));
  sched->run();
}

static
void
test_timeout_dont()
{
  Fixture f;

  reactor::Signal s;
  reactor::Thread t(*sched, "timeout",
                    boost::bind(timeout, boost::ref(s), true));
  reactor::Thread p(*sched, "poker",
                    boost::bind(timeout_send, boost::ref(s)));
  sched->run();
}

/*----------------.
| Timeout aborted |
`----------------*/

// Check abort + timeout is not an issue.

static
void
connor()
{
  reactor::Semaphore s(0);
  reactor::Scheduler::scheduler()->current()->wait(
    s, boost::posix_time::milliseconds(1));
}

static
void
schwarzy()
{
  reactor::Scheduler::scheduler()->terminate();
  ::usleep(10);
}

static
void
test_timeout_aborted()
{
  reactor::Scheduler sched;

  boost::asio::deadline_timer(sched.io_service());
  reactor::Thread t1(sched, "John", &connor);
  reactor::Thread t2(sched, "Terminator", &schwarzy);
  sched.run();
}

/*--------------.
| Timeout threw |
`--------------*/

static
void
test_timeout_threw()
{
  reactor::Scheduler sched;
  reactor::Semaphore sem(0);

  reactor::Thread thrower(sched, "thrower", [&] {
      thrower.wait(sem);
      throw BeaconException();
    });
  reactor::Thread waiter(sched, "waiter", [&] {
      sem.release();
      waiter.wait(thrower, 100_ms);
    });

  try
  {
    sched.run();
  }
  catch (BeaconException const&)
  {}
}

/*---------------------.
| Timeout and finished |
`---------------------*/

static
void
test_timeout_finished()
{
  reactor::Scheduler sched;

  reactor::Thread waiter(sched, "waiter", [&] {
      reactor::Sleep s(sched, 10_ms);
      s.start();
      // Block the IO service to make sure the task times out in the same cycle
      // it finishes.
      sched.io_service().post([] { ::usleep(200000); });
      _wait(s, 11_ms);
    });
  sched.run();
}

/*--------.
| VThread |
`--------*/

static
int
answer()
{
  return 42;
}

static
void
test_vthread()
{
  Fixture f;

  reactor::VThread<int> t(*sched, "return value", answer);
  BOOST_CHECK_THROW(t.result(), elle::Exception);
  sched->run();
  BOOST_CHECK_EQUAL(t.result(), 42);
}

/*------------.
| Multithread |
`------------*/

static
void
waker(reactor::Signal& s)
{
  // FIXME: sleeps suck

  // Make sure the scheduler is sleeping.
  sleep(1);
  reactor::Thread w(*sched, "waker",
                    boost::bind(&reactor::Signal::signal, &s));
  // Make sure the scheduler is done.
  while (!w.done())
    sleep(1);
}

static
void
test_multithread_spawn_wake()
{
  Fixture f;
  reactor::Signal sig;
  typedef bool (reactor::Thread::*F)(reactor::Waitable&, reactor::DurationOpt);
  reactor::Thread keeper(*sched, "keeper",
                         boost::bind(static_cast<F>(&reactor::Thread::wait),
                                     &keeper, boost::ref(sig),
                                     reactor::DurationOpt()));
  boost::thread s(boost::bind(waker, boost::ref(sig)));
  sched->run();
  s.join();
}

static
int
spawned(reactor::Signal& s)
{
  s.signal();
  return 42;
}

static
void
spawn(reactor::Signal& s,
      int& res)
{
  res = sched->mt_run<int>("spawned", boost::bind(spawned, boost::ref(s)));
}

static
void
spawner()
{
  reactor::Signal s;
  int res = 0;
  boost::thread spawner(boost::bind(spawn, boost::ref(s), boost::ref(res)));
  _wait(s);
  spawner.join();
  BOOST_CHECK_EQUAL(res, 42);
}

static
void
test_multithread_run()
{
  Fixture f;

  reactor::Thread t(*sched, "spawner", spawner);
  sched->run();
}

static
void
test_multithread_run_exception()
{
  reactor::Scheduler sched;

  reactor::Signal terminate;
  reactor::Thread holder(sched, "spawner",
                         [&] () { sched.current()->wait(terminate); });
  boost::thread runner(std::bind(&reactor::Scheduler::run, &sched));
  BOOST_CHECK_THROW(
    sched.mt_run<void>("thrower", [] () { throw BeaconException(); }),
    BeaconException);
  BOOST_CHECK_THROW(
    sched.mt_run<int>("thrower", [] () -> int { throw BeaconException(); }),
    BeaconException);
  sched.mt_run<void>("terminator", [&] () { terminate.signal(); });
  runner.join();
}

static
void
test_multithread_deadlock_assert()
{
  reactor::Scheduler sched;

  reactor::Thread t(sched, "thread",
                    [&]
                    {
                      BOOST_CHECK_THROW(sched.mt_run<void>("noop", [] {}),
                                        std::exception);
                    });
  sched.run();
}

/*----------.
| Semaphore |
`----------*/

static
void
semaphore_noblock_wait(reactor::Semaphore& s)
{
  BOOST_CHECK_EQUAL(s.count(), 2);
  _wait(s);
  BOOST_CHECK_EQUAL(s.count(), 1);
  _wait(s);
  BOOST_CHECK_EQUAL(s.count(), 0);
}

static
void
test_semaphore_noblock()
{
  Fixture f;
  reactor::Semaphore s(2);
  reactor::Thread wait(*sched, "wait",
                       boost::bind(&semaphore_noblock_wait, boost::ref(s)));
  sched->run();
}

static
void
semaphore_block_wait(reactor::Semaphore& s)
{
  BOOST_CHECK_EQUAL(s.count(), 0);
  _wait(s);
  BOOST_CHECK_EQUAL(s.count(), 0);
}

static
void
semaphore_block_post(reactor::Semaphore& s)
{
  yield();
  yield();
  yield();
  BOOST_CHECK_EQUAL(s.count(), -1);
  s.release();
  BOOST_CHECK_EQUAL(s.count(), 0);
}

static
void
test_semaphore_block()
{
  Fixture f;
  reactor::Semaphore s;
  reactor::Thread wait(*sched, "wait",
                       boost::bind(&semaphore_block_wait, boost::ref(s)));
  reactor::Thread post(*sched, "post",
                       boost::bind(&semaphore_block_post, boost::ref(s)));
  sched->run();
}

static
void
test_semaphore_multi()
{
  reactor::Scheduler sched;
  reactor::Semaphore s;
  int step = 0;

  auto multi_wait = [&] { _wait(s); ++step; };

  reactor::Thread wait1(sched, "wait1", multi_wait);
  reactor::Thread wait2(sched, "wait2", multi_wait);
  reactor::Thread post(sched, "post", [&] {
      yield();
      yield();
      yield();
      BOOST_CHECK_EQUAL(s.count(), -2);
      BOOST_CHECK_EQUAL(step, 0);
      s.release();
      yield();
      yield();
      BOOST_CHECK_EQUAL(s.count(), -1);
      BOOST_CHECK_EQUAL(step, 1);
      s.release();
      yield();
      yield();
      BOOST_CHECK_EQUAL(s.count(), 0);
      BOOST_CHECK_EQUAL(step, 2);
    });
  sched.run();
}

/*------.
| Mutex |
`------*/

static const int mutex_yields = 32;

static
void
mutex_count(int& i,
            reactor::Mutex& mutex,
            int yields)
{
  int count = 0;
  int prev = -1;
  while (count < mutex_yields)
  {
    {
      reactor::Lock lock(mutex);
      // For now, mutex do guarantee fairness between lockers.
      //BOOST_CHECK_NE(i, prev);
      (void)prev;
      BOOST_CHECK_EQUAL(i % 2, 0);
      ++i;
      for (int c = 0; c < yields; ++c)
      {
        ++count;
        yield();
      }
      ++i;
      prev = i;
    }
    yield();
  }
}

static
void
test_mutex()
{
  Fixture f;
  reactor::Mutex mutex;
  int step = 0;
  reactor::Thread c1(*sched, "counter1",
                     boost::bind(&mutex_count,
                                 boost::ref(step), boost::ref(mutex), 1));
  reactor::Thread c2(*sched, "counter2",
                     boost::bind(&mutex_count,
                                 boost::ref(step), boost::ref(mutex), 1));
  reactor::Thread c3(*sched, "counter3",
                     boost::bind(&mutex_count,
                                 boost::ref(step), boost::ref(mutex), 1));
  sched->run();
}

/*--------.
| RWMutex |
`--------*/

static
void
rw_mutex_read(reactor::RWMutex& mutex,
              int& step)
{
  reactor::Lock lock(mutex);
  ++step;
  yield();
  BOOST_CHECK_EQUAL(step, 3);
}

static
void
test_rw_mutex_multi_read()
{
  reactor::Scheduler sched;
  reactor::RWMutex mutex;
  int step = 0;
  reactor::Thread r1(sched, "reader1",
                     boost::bind(rw_mutex_read,
                                 boost::ref(mutex), boost::ref(step)));
  reactor::Thread r2(sched, "reader2",
                     boost::bind(rw_mutex_read,
                                 boost::ref(mutex), boost::ref(step)));
  reactor::Thread r3(sched, "reader3",
                     boost::bind(rw_mutex_read,
                                 boost::ref(mutex), boost::ref(step)));
  sched.run();
}

static
void
rw_mutex_write(reactor::RWMutex& mutex,
               int& step)
{
  reactor::Lock lock(mutex.write());
  ++step;
  int prev = step;
  yield();
  BOOST_CHECK_EQUAL(step, prev);
}

static
void
test_rw_mutex_multi_write()
{
  reactor::Scheduler sched;
  reactor::RWMutex mutex;
  int step = 0;
  reactor::Thread r1(sched, "writer1",
                     boost::bind(rw_mutex_write,
                                 boost::ref(mutex), boost::ref(step)));
  reactor::Thread r2(sched, "writer2",
                     boost::bind(rw_mutex_write,
                                 boost::ref(mutex), boost::ref(step)));
  reactor::Thread r3(sched, "writer3",
                     boost::bind(rw_mutex_write,
                                 boost::ref(mutex), boost::ref(step)));
  sched.run();
}

static
void
rw_mutex_both_read(reactor::RWMutex& mutex,
                   int& step)
{
  reactor::Lock lock(mutex);
  int v = step;
  BOOST_CHECK_EQUAL(v % 2, 0);
  BOOST_CHECK_EQUAL(step, v);
  yield();
  BOOST_CHECK_EQUAL(step, v);
  yield();
  BOOST_CHECK_EQUAL(step, v);
}

static
void
rw_mutex_both_write(reactor::RWMutex& mutex,
                    int& step)
{
  reactor::Lock lock(mutex.write());
  ++step;
  yield();
  yield();
  ++step;
  BOOST_CHECK_EQUAL(step % 2, 0);
}

static
void
test_rw_mutex_both()
{
  reactor::Scheduler sched;
  reactor::RWMutex mutex;
  int step = 0;
  reactor::Thread r1(sched, "reader1",
                     boost::bind(rw_mutex_both_read,
                                 boost::ref(mutex), boost::ref(step)));
  reactor::Thread r2(sched, "reader2",
                     boost::bind(rw_mutex_both_read,
                                 boost::ref(mutex), boost::ref(step)));
  sched.step();


  reactor::Thread w1(sched, "writer1",
                     boost::bind(rw_mutex_both_write,
                                 boost::ref(mutex), boost::ref(step)));

  reactor::Thread w2(sched, "writer2",
                     boost::bind(rw_mutex_both_write,
                                 boost::ref(mutex), boost::ref(step)));
  while (!r1.done())
    sched.step();
  BOOST_CHECK(r2.done());
  sched.step();

  reactor::Thread r3(sched, "reader3",
                     boost::bind(rw_mutex_both_read,
                                 boost::ref(mutex), boost::ref(step)));
  reactor::Thread r4(sched, "reader4",
                     boost::bind(rw_mutex_both_read,
                                 boost::ref(mutex), boost::ref(step)));
  while (!w1.done() || !w2.done())
    sched.step();

  sched.step();


  reactor::Thread w3(sched, "writer2",
                     boost::bind(rw_mutex_both_write,
                                 boost::ref(mutex), boost::ref(step)));

  reactor::Thread w4(sched, "writer4",
                     boost::bind(rw_mutex_both_write,
                                 boost::ref(mutex), boost::ref(step)));

  sched.run();
}

/*--------.
| Storage |
`--------*/

static
void
storage(reactor::LocalStorage<int>& val,
        int start)
{
  val.Get() = start;
  yield();
  BOOST_CHECK_EQUAL(val.Get(), start);
  val.Get()++;
  yield();
  BOOST_CHECK_EQUAL(val.Get(), start + 1);
}

static
void
test_storage()
{
  reactor::Scheduler sched;
  reactor::LocalStorage<int> val;

  reactor::Thread t1(sched, "1", boost::bind(storage, boost::ref(val), 0));
  reactor::Thread t2(sched, "2", boost::bind(storage, boost::ref(val), 1));
  reactor::Thread t3(sched, "3", boost::bind(storage, boost::ref(val), 2));
  reactor::Thread t4(sched, "4", boost::bind(storage, boost::ref(val), 3));

  sched.run();
}

static
void
test_storage_multithread()
{
  reactor::LocalStorage<int> val;

  auto coro_action = [&]()
    {
      val.Get() = 0;
    };

  auto action = [&]()
    {
      using namespace boost::posix_time;
      ptime deadline = microsec_clock::local_time() + milliseconds(500);
      reactor::Scheduler sched;
      while (microsec_clock::local_time() < deadline)
      {
        reactor::Thread t(sched, "1", coro_action);
        sched.run();
      }
    };
  std::vector<std::thread> threads;
  try
  {
    for (int i = 0; i < 64; ++i)
      threads.push_back(std::thread(action));
  }
  catch (...)
  {
    for (auto& thread: threads)
      thread.join();
    throw;
  }
  for (auto& thread: threads)
    thread.join();
}

/*------------.
| Multithread |
`------------*/

static
void
test_multithread()
{
  reactor::Scheduler sched1;
  reactor::Scheduler sched2;
  reactor::Scheduler sched3;

  auto action = [&]()
  {
    for (int i = 0; i < 64; ++i)
      reactor::Scheduler::scheduler()->current()->yield();
  };

  reactor::Thread thread1(sched1, "Thread 1", action);
  reactor::Thread thread2(sched2, "Thread 2", action);
  reactor::Thread thread3(sched3, "Thread 3", action);

  std::thread t1([&](){sched1.run();});
  std::thread t2([&](){sched2.run();});
  std::thread t3([&](){sched2.run();});

  t1.join();
  t2.join();
  t3.join();
}

/*----------.
| Terminate |
`----------*/

static
void
except_gen()
{
  throw BeaconException();
}

static
void
thread_exception_test()
{
  Fixture f;
  reactor::Thread thread(*sched, "Exception Tester", &except_gen);
  bool exception_thrown = false;

  try
  {
    sched->run();
  }
  catch (const BeaconException& e)
  {
    BOOST_CHECK_EQUAL(e.what(), "beacon");
    exception_thrown = true;
  }

  BOOST_CHECK(exception_thrown);
  BOOST_CHECK_EQUAL(thread.state(), reactor::Thread::state::done);
}

/*----------------.
| Terminate yield |
`----------------*/

static
void
terminate_scheduler()
{
  auto& sched = *reactor::Scheduler::scheduler();

  sched.terminate();
}

static
void
terminate_yield_thread(bool& beacon)
{
  auto& sched = *reactor::Scheduler::scheduler();
  auto& self = *sched.current();

  try
  {
    self.yield();
  }
  catch (...)
  {
    self.yield();
    beacon = true;
    throw;
  }
  BOOST_CHECK(false);
}

static
void
test_terminate_yield()
{
  reactor::Scheduler sched;

  bool beacon = false;
  reactor::Thread t(sched, "Thread", std::bind(&terminate_yield_thread,
                                               std::ref(beacon)));
  reactor::Thread term(sched, "Terminate", &terminate_scheduler);

  sched.run();
  BOOST_CHECK(beacon);
}


/*--------------.
| Terminate now |
`--------------*/

static
void
terminated(bool& terminated)
{
  auto& sched = *reactor::Scheduler::scheduler();
  auto& self = *sched.current();

  terminated = false;
  try
  {
    while (true)
    {
      self.yield();
    }
  }
  catch (...)
  {
    terminated = true;
    self.sleep(boost::posix_time::milliseconds(10));
    throw;
  }
}


static
void
terminate_now(reactor::Thread& t, bool& terminated)
{
  t.terminate_now();
  BOOST_CHECK(terminated);
}

static
void
test_terminate_now()
{
  reactor::Scheduler sched;
  bool beacon = false;
  reactor::Thread t(sched, "terminated", std::bind(&terminated,
                                                   std::ref(beacon)));
  reactor::Thread terminate(sched, "terminate", std::bind(&terminate_now,
                                                          std::ref(t),
                                                          std::ref(beacon)));
  sched.run();
}

/*------------------------.
| Terminate now destroyed |
`------------------------*/

static
void
terminate_now_destroyed_t1(void)
{
  auto& sched = *reactor::Scheduler::scheduler();
  auto& self = *sched.current();

  // Ignore the first Terminate (sent by t2) and catch the second one (by t4).
  try
  {
    while (true)
      self.yield();
  }
  catch (...)
  {}

  while (true)
    self.yield();
}

static
void
terminate_now_destroyed_t2(reactor::Thread& t1)
{
  t1.terminate_now();
  BOOST_FAIL("terminate_now should have failed (killed by t3 during wait)");
}

static
void
terminate_now_destroyed_t3(reactor::Thread& t2)
{
  t2.terminate_now();
}

static
void
terminate_now_destroyed_t4(reactor::Thread& t1,
                           reactor::Thread& t2,
                           reactor::Thread& t3)
{
  auto& sched = *reactor::Scheduler::scheduler();
  auto& self = *sched.current();

  while (true)
  {
    // The thread t2 did not wait for t1 to end before dying.
    if (t1.state() == reactor::Thread::state::running &&
        t2.state() == reactor::Thread::state::done)
      BOOST_FAIL("2nd thread did not wait for 1st thread to die before dying");
    // If t2 is effectively waiting for t1 to die and t3 for t2 to die
    // then the test has passed.
    else if (t1.state() == reactor::Thread::state::running &&
             t2.state() == reactor::Thread::state::frozen &&
             t3.state() == reactor::Thread::state::frozen)
    {
      t1.terminate_now();
      break;
    }

    self.yield();
  }
}

static
void
test_terminate_now_destroyed()
{
  reactor::Scheduler sched;

  reactor::Thread t1(sched, "t1", &terminate_now_destroyed_t1);
  reactor::Thread t2(sched, "t2", std::bind(&terminate_now_destroyed_t2,
                                            std::ref(t1)));
  reactor::Thread t3(sched, "t3", std::bind(&terminate_now_destroyed_t3,
                                            std::ref(t2)));
  reactor::Thread t4(sched, "t4", std::bind(&terminate_now_destroyed_t4,
                                            std::ref(t1),
                                            std::ref(t2),
                                            std::ref(t3)));

  sched.run();
}

/*-----------------------.
| Terminate now disposed |
`-----------------------*/

static
void
test_terminate_now_disposed()
{
  reactor::Scheduler sched;
  bool beacon = false;
  auto* t = new reactor::Thread(sched, "terminated",
                                std::bind(&terminated, std::ref(beacon)), true);
  reactor::Thread terminate(sched, "terminate", std::bind(&terminate_now,
                                                          std::ref(*t),
                                                          std::ref(beacon)));


  sched.run();
}

/*-----------------------.
| Terminate now starting |
`-----------------------*/

static
void
test_terminate_now_starting()
{
  bool beacon = true;
  reactor::Scheduler sched;

  reactor::Thread starting(sched, "starting", [] {});
  reactor::Thread terminate(sched, "terminate", std::bind(&terminate_now,
                                                          std::ref(starting),
                                                          std::ref(beacon)));
  sched.run();
}

/*------------------------.
| Terminate now scheduled |
`------------------------*/

static
void
victim()
{

}

static
void
killer(reactor::Thread& v)
{
  v.terminate_now();
}

static
void
test_terminate_now_scheduled()
{
  reactor::Scheduler sched;

  // Make sure v is run after k in the first round, to check if terminating it
  // removes it from the running thread for this round.
  reactor::Thread* g = nullptr;
  reactor::Thread k(sched, "Killer", [&]() { killer(*g); });
  reactor::Thread v(sched, "Victim", &victim);
  g = &v;

  sched.run();
}

/*-----------------.
| Exception escape |
`-----------------*/

static
void
test_exception_escape()
{
  reactor::Scheduler sched;

  reactor::Thread t(sched, "thrower", except_gen);

  BOOST_CHECK_THROW(sched.run(), BeaconException);
}


/*----------------------------.
| Exception escape collateral |
`----------------------------*/

static
void
exception_escape_collateral(int& beacon)
{
  auto& sched = *reactor::Scheduler::scheduler();
  auto& current = *sched.current();

  ++beacon;
  try
  {
    current.yield();
  }
  catch (...)
  {
    ++beacon;
    current.yield();
    ++beacon;
  }
}

static
void
test_exception_escape_collateral()
{
  reactor::Scheduler sched;
  int beacon = 0;

  reactor::Thread t1(sched, "collateral",
                     std::bind(exception_escape_collateral, std::ref(beacon)));
  reactor::Thread t2(sched, "thrower", except_gen);

  BOOST_CHECK_THROW(sched.run(), BeaconException);
  BOOST_CHECK_EQUAL(beacon, 3);
}


/*-----------------.
| IO service throw |
`-----------------*/

static
void
poster(bool& beacon)
{
  auto& sched = *reactor::Scheduler::scheduler();
  sched.io_service().post(&except_gen);
  sched.current()->yield();
  beacon = true;
}

static
void
test_io_service_throw()
{
  bool beacon = false;
  reactor::Scheduler sched;
  reactor::Thread thread(sched, "Poster", std::bind(&poster, std::ref(beacon)));
  BOOST_CHECK_THROW(sched.run(), BeaconException);
  BOOST_CHECK(!beacon);
}

/*-----------.
| Background |
`-----------*/

namespace background
{
  static
  void
  operation()
  {
    reactor::Scheduler sched;
    int i = 0;
    reactor::Thread thread(
      sched, "main",
      [&i]
      {
        reactor::background([] { ::sleep(1); });
        BOOST_CHECK_EQUAL(i, 3);
      });
    reactor::Thread counter(
      sched, "counter",
      [&]
      {
        ++i;
        yield();
        ++i;
        yield();
        ++i;
      });
    sched.run();
  }

  static
  void
  operations()
  {
    reactor::Scheduler sched;
    static int const iterations = 16;
    reactor::Thread main(
      sched, "main",
      [&]
      {
        // Run it three times to check the thread pool doesn't exceed 16.
        for (int run = 0; run < 3; ++run)
        {
          int count = 0;
          std::vector<reactor::Thread*> threads;
          for (int i = 0; i < iterations; ++i)
            threads.push_back(
              new reactor::Thread(sched, elle::sprintf("thread %s", i),
                                  [&count]
                                  {
                                    reactor::background([] { ::sleep(1); });
                                    ++count;
                                  }));
          auto start = std::chrono::system_clock::now();
          sched.current()->wait(reactor::Waitables(begin(threads),
                                                   end(threads)));
          for (auto thread: threads)
            delete thread;
          auto duration = std::chrono::system_clock::now() - start;
          BOOST_CHECK_EQUAL(count, iterations);
          BOOST_CHECK_EQUAL(sched.background_pool_size(), iterations);
          BOOST_CHECK(duration < std::chrono::seconds(2));
        }
      });
    sched.run();
  }

  static
  void
  exception()
  {
    reactor::Scheduler sched;
    reactor::Thread thread(
      sched, "main",
      [&]
      {
        BOOST_CHECK_THROW(reactor::background([] { throw BeaconException(); }),
                          std::exception);
        BOOST_CHECK_THROW(reactor::background([] { throw BeaconException(); }),
                          std::exception);
        BOOST_CHECK_THROW(reactor::background([] { throw BeaconException(); }),
                          std::exception);
        BOOST_CHECK_EQUAL(sched.background_pool_size(), 1);
      });
    sched.run();
  }

  static
  void
  aborted()
  {
    reactor::Scheduler sched;
    reactor::Thread main(
      sched, "main",
      [&]
      {
        auto start = std::chrono::system_clock::now();
        reactor::background([] { ::sleep(1); });
        auto duration = std::chrono::system_clock::now() - start;
        BOOST_CHECK(duration < std::chrono::seconds(1));
      });
    reactor::Thread kill(
      sched, "kill",
      [&]
      {
        main.terminate();
      });
    sched.run();
  }

  static
  void
  aborted_throw()
  {
    reactor::Scheduler sched;
    reactor::Thread main(
      sched, "main",
      [&]
      {
        auto start = std::chrono::system_clock::now();
        reactor::background([] { ::sleep(1); throw BeaconException(); });
        auto duration = std::chrono::system_clock::now() - start;
        BOOST_CHECK(duration < std::chrono::seconds(1));
      });
    reactor::Thread kill(
      sched, "kill",
      [&]
      {
        main.terminate();
      });
    sched.run();
  }
}

/*--------.
| Signals |
`--------*/

namespace system_signals
{
  static
  void
  terminate()
  {
    reactor::Scheduler sched;
    reactor::Thread t(sched, "main", [&t] ()
                      {
                        ::kill(::getpid(), SIGINT);
                        t.reactor::Waitable::wait();
                      });
    sched.signal_handle(SIGINT, [&sched] { sched.terminate(); });
    sched.run();
  }
}

/*-----.
| Main |
`-----*/

static
bool
test_suite()
{
  boost::unit_test::test_suite* basics = BOOST_TEST_SUITE("Basics");
  boost::unit_test::framework::master_test_suite().add(basics);
  basics->add(BOOST_TEST_CASE(test_basics_one));
  basics->add(BOOST_TEST_CASE(test_basics_interleave));

  boost::unit_test::test_suite* multithread = BOOST_TEST_SUITE("Multithread");
  multithread->add(BOOST_TEST_CASE(test_multithread));

  boost::unit_test::test_suite* signals = BOOST_TEST_SUITE("Signals");
  boost::unit_test::framework::master_test_suite().add(signals);
  signals->add(BOOST_TEST_CASE(test_signals_one_on_one));
  signals->add(BOOST_TEST_CASE(test_signals_one_on_two));
  signals->add(BOOST_TEST_CASE(test_signals_two_on_one));
  signals->add(BOOST_TEST_CASE(test_signals_timeout));

  boost::unit_test::test_suite* barrier = BOOST_TEST_SUITE("Barrier");
  boost::unit_test::framework::master_test_suite().add(barrier);
  barrier->add(BOOST_TEST_CASE(barrier_closed));
  barrier->add(BOOST_TEST_CASE(barrier_opened));

  // Scope
  {
    boost::unit_test::test_suite* scope = BOOST_TEST_SUITE("scope");
    boost::unit_test::framework::master_test_suite().add(scope);
    auto empty = &scope::empty;
    scope->add(BOOST_TEST_CASE(empty));
    auto wait = &scope::wait;
    scope->add(BOOST_TEST_CASE(wait));
    auto exception = &scope::exception;
    scope->add(BOOST_TEST_CASE(exception));
    auto multiple_exception = &scope::multiple_exception;
    scope->add(BOOST_TEST_CASE(multiple_exception));
    auto terminate = &scope::terminate;
    scope->add(BOOST_TEST_CASE(terminate));
    auto terminate_all = &scope::terminate_all;
    scope->add(BOOST_TEST_CASE(terminate_all));
  }

  boost::unit_test::test_suite* sleep = BOOST_TEST_SUITE("Sleep");
  boost::unit_test::framework::master_test_suite().add(sleep);
  sleep->add(BOOST_TEST_CASE(test_sleep_interleave));
  sleep->add(BOOST_TEST_CASE(test_sleep_timing));

  boost::unit_test::test_suite* join = BOOST_TEST_SUITE("Join");
  boost::unit_test::framework::master_test_suite().add(join);
  join->add(BOOST_TEST_CASE(test_join));
  join->add(BOOST_TEST_CASE(test_join_multiple));
  join->add(BOOST_TEST_CASE(test_join_timeout));

  boost::unit_test::test_suite* terminate = BOOST_TEST_SUITE("Terminate");
  boost::unit_test::framework::master_test_suite().add(terminate);
  terminate->add(BOOST_TEST_CASE(test_terminate_yield));
  terminate->add(BOOST_TEST_CASE(test_terminate_now));
  terminate->add(BOOST_TEST_CASE(test_terminate_now_destroyed));
  terminate->add(BOOST_TEST_CASE(test_terminate_now_disposed));
  terminate->add(BOOST_TEST_CASE(test_terminate_now_starting));
  terminate->add(BOOST_TEST_CASE(test_terminate_now_scheduled));
  terminate->add(BOOST_TEST_CASE(test_exception_escape));
  terminate->add(BOOST_TEST_CASE(test_exception_escape_collateral));

  boost::unit_test::test_suite* timeout = BOOST_TEST_SUITE("Timeout");
  boost::unit_test::framework::master_test_suite().add(timeout);
  timeout->add(BOOST_TEST_CASE(test_timeout_do));
  timeout->add(BOOST_TEST_CASE(test_timeout_dont));
  timeout->add(BOOST_TEST_CASE(test_timeout_aborted));
  timeout->add(BOOST_TEST_CASE(test_timeout_threw));
  timeout->add(BOOST_TEST_CASE(test_timeout_finished));

  boost::unit_test::test_suite* vthread = BOOST_TEST_SUITE("Return value");
  boost::unit_test::framework::master_test_suite().add(vthread);
  vthread->add(BOOST_TEST_CASE(test_vthread));

  boost::unit_test::test_suite* mt = BOOST_TEST_SUITE("Multithreading");
  boost::unit_test::framework::master_test_suite().add(mt);
  mt->add(BOOST_TEST_CASE(test_multithread_spawn_wake));
  mt->add(BOOST_TEST_CASE(test_multithread_run));
  mt->add(BOOST_TEST_CASE(test_multithread_run_exception));
  mt->add(BOOST_TEST_CASE(test_multithread_deadlock_assert));

  boost::unit_test::test_suite* sem = BOOST_TEST_SUITE("Semaphore");
  boost::unit_test::framework::master_test_suite().add(sem);
  sem->add(BOOST_TEST_CASE(test_semaphore_noblock));
  sem->add(BOOST_TEST_CASE(test_semaphore_block));
  sem->add(BOOST_TEST_CASE(test_semaphore_multi));

  boost::unit_test::test_suite* mtx = BOOST_TEST_SUITE("Mutex");
  boost::unit_test::framework::master_test_suite().add(mtx);
  mtx->add(BOOST_TEST_CASE(test_mutex));

  boost::unit_test::test_suite* rwmtx = BOOST_TEST_SUITE("RWMutex");
  boost::unit_test::framework::master_test_suite().add(rwmtx);
  rwmtx->add(BOOST_TEST_CASE(test_rw_mutex_multi_read));
  rwmtx->add(BOOST_TEST_CASE(test_rw_mutex_multi_write));
  rwmtx->add(BOOST_TEST_CASE(test_rw_mutex_both));

  boost::unit_test::test_suite* storage = BOOST_TEST_SUITE("Storage");
  boost::unit_test::framework::master_test_suite().add(storage);
  storage->add(BOOST_TEST_CASE(test_storage));
  storage->add(BOOST_TEST_CASE(test_storage_multithread));

  boost::unit_test::test_suite* thread_exception =
    BOOST_TEST_SUITE("Thread exception");
  boost::unit_test::framework::master_test_suite().add(thread_exception);
  thread_exception->add(BOOST_TEST_CASE(thread_exception_test));

  boost::unit_test::test_suite* io_service = BOOST_TEST_SUITE("IO service");
  boost::unit_test::framework::master_test_suite().add(io_service);
  io_service->add(BOOST_TEST_CASE(test_io_service_throw));

  boost::unit_test::test_suite* background = BOOST_TEST_SUITE("background");
  boost::unit_test::framework::master_test_suite().add(background);
  {
    using namespace background;
    background->add(BOOST_TEST_CASE(operation));
    background->add(BOOST_TEST_CASE(operations));
    background->add(BOOST_TEST_CASE(exception));
    background->add(BOOST_TEST_CASE(aborted));
    background->add(BOOST_TEST_CASE(aborted_throw));
  }

  {
    boost::unit_test::test_suite* system_signals =
      BOOST_TEST_SUITE("system_signals");
    boost::unit_test::framework::master_test_suite().add(system_signals);
    auto terminate = system_signals::terminate;
    system_signals->add(BOOST_TEST_CASE(terminate), 0, 1);
  }

  return true;
}

int
main(int argc,
     char** argv)
{
  return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}
