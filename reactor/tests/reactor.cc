#include <condition_variable>
#include <memory>
#include <mutex>

#include "reactor.hh"

#include <elle/finally.hh>
#include <elle/test.hh>

#include <reactor/BackgroundFuture.hh>
#include <reactor/Barrier.hh>
#include <reactor/Channel.hh>
#include <reactor/MultiLockBarrier.hh>
#include <reactor/OrWaitable.hh>
#include <reactor/Scope.hh>
#include <reactor/TimeoutGuard.hh>
#include <reactor/asio.hh>
#include <reactor/duration.hh>
#include <reactor/exception.hh>
#include <reactor/for-each.hh>
#include <reactor/mutex.hh>
#include <reactor/rw-mutex.hh>
#include <reactor/semaphore.hh>
#include <reactor/signal.hh>
#include <reactor/sleep.hh>
#include <reactor/storage.hh>
#include <reactor/thread.hh>
#include <reactor/timer.hh>


ELLE_LOG_COMPONENT("Test");

/*-----------------.
| Global shortcuts |
`-----------------*/


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
usleep(int usec)
{
  ::Sleep(usec / 1000.0);
}
#endif

/*-------.
| Basics |
`-------*/

static
void
test_basics_one()
{
  reactor::Scheduler sched;
  int step = 0;
  reactor::Thread t(
    sched,
    "coro",
    [&]
    {
      BOOST_CHECK_EQUAL(step, 0);
      ++step;
      reactor::yield();
      BOOST_CHECK_EQUAL(step, 1);
      ++step;
    });
  sched.run();
  BOOST_CHECK_EQUAL(step, 2);
}

static
void
coro1(int& step)
{
  BOOST_CHECK(step == 0 || step == 1);
  ++step;
  reactor::yield();
  BOOST_CHECK(step == 2 || step == 3);
  ++step;
  reactor::yield();
  BOOST_CHECK(step == 4);
  ++step;
  reactor::yield();
  BOOST_CHECK(step == 5);
}

static
void
coro2(int& step)
{
  BOOST_CHECK(step == 0 || step == 1);
  ++step;
  reactor::yield();
  BOOST_CHECK(step == 2 || step == 3);
  ++step;
}

static
void
test_basics_interleave()
{
  reactor::Scheduler sched;

  int step = 0;
  reactor::Thread c1(sched, "1", boost::bind(coro1, boost::ref(step)));
  reactor::Thread c2(sched, "2", boost::bind(coro2, boost::ref(step)));
  sched.run();
  BOOST_CHECK_EQUAL(step, 5);
}

static
void
nested_schedulers()
{
  reactor::Scheduler outer;

  reactor::Thread t(
    outer, "outer",
    [&]
    {
      BOOST_CHECK(reactor::Scheduler::scheduler() == &outer);
      reactor::Scheduler inner;
      reactor::Thread t(
        inner, "inner",
        [&]
        {
          BOOST_CHECK(reactor::Scheduler::scheduler() == &inner);
        });
      inner.run();
      BOOST_CHECK(reactor::Scheduler::scheduler() == &outer);
    });
  BOOST_CHECK(reactor::Scheduler::scheduler() == 0);
  outer.run();
  BOOST_CHECK(reactor::Scheduler::scheduler() == 0);
}

ELLE_TEST_SCHEDULED(managed)
{
  reactor::Thread t(
    "thrower",
    [&]
    {
      throw BeaconException();
    },
    reactor::Thread::managed = true);
  BOOST_CHECK_THROW(reactor::wait(t), BeaconException);
  BOOST_CHECK_THROW(reactor::wait(t), BeaconException);
}

ELLE_TEST_SCHEDULED_THROWS(non_managed, BeaconException)
{
  reactor::Thread thrower(
    "thrower",
    [&]
    {
      throw BeaconException();
    });
  try
  {
    reactor::sleep();
  }
  catch (...)
  {
    BOOST_CHECK_NO_THROW(reactor::wait(thrower));
    throw;
  }
}

ELLE_TEST_SCHEDULED(unique_ptr)
{
  ELLE_LOG("test null managed pointer")
  {
    reactor::Thread::unique_ptr t;
  }
  ELLE_LOG("terminate thread upon release")
  {
    bool beacon = false;
    reactor::Barrier sleeping;
    reactor::Thread::unique_ptr t(
      new reactor::Thread(
        "waiter",
        [&]
        {
          try
          {
            sleeping.open();
            reactor::sleep();
            BOOST_FAIL("NOT POSSIBLE");
          }
          catch (...)
          {
            beacon = true;
            throw;
          }
        }));
    reactor::wait(sleeping);
    t.reset();
    BOOST_CHECK(beacon);
  }
  ELLE_LOG("terminate disposed thread upon release")
  {
    bool beacon = false;
    reactor::Barrier sleeping;
    reactor::Thread::unique_ptr t(
      new reactor::Thread(
        "waiter",
        [&]
        {
          try
          {
            sleeping.open();
            reactor::sleep();
            BOOST_FAIL("NOT POSSIBLE");
          }
          catch (...)
          {
            beacon = true;
            throw;
          }
        },
        reactor::Thread::dispose = true));
    reactor::wait(sleeping);
    t.reset();
    BOOST_CHECK(beacon);
  }
  ELLE_LOG("release disposed thread")
  {
    reactor::Barrier run;
    reactor::Thread::unique_ptr t(
      new reactor::Thread(
        "waiter",
        [&]
        {
          run.open();
        },
        reactor::Thread::dispose = true));
    reactor::wait(run);
    BOOST_CHECK(!t);
    t.reset();
  }
  ELLE_LOG("destroy pointer to alive disposed thread")
  {
    reactor::Barrier go;
    auto raw = new reactor::Thread(
      "waiter",
      [&]
      {
        reactor::wait(go);
      },
      reactor::Thread::dispose = true);
    auto t = new reactor::Thread::unique_ptr(raw);
    t->release();
    delete t;
    go.open();
    reactor::wait(*raw);
  }
}

ELLE_TEST_SCHEDULED(deadlock)
{
  reactor::Thread::unique_ptr starting;
  reactor::Thread killer(
    "killer",
    [&]
    {
      starting->terminate();
    });
  reactor::yield();
  starting.reset(new reactor::Thread("starting", [] { reactor::sleep(); }));
  reactor::wait(*starting);
}

/*-----.
| Wait |
`-----*/

namespace waitable
{
  class ExceptionNoWait:
    public reactor::Waitable
  {
  public:
    ExceptionNoWait()
    {
      this->_raise<BeaconException>();
    }

    virtual
    bool
    _wait(reactor::Thread*, Waker const&) override
    {
      return false;
    }
  };

  ELLE_TEST_SCHEDULED(exception_no_wait)
  {
    ExceptionNoWait waitable;
    BOOST_CHECK_THROW(reactor::wait(waitable), BeaconException);
  }

  ELLE_TEST_SCHEDULED(logical_or)
  {
    reactor::Barrier a("A");
    a.open();
    reactor::Barrier b("B");
    b.open();
    ELLE_LOG("both open")
    {
      auto w = a || b;
      reactor::wait(w);
    }
    ELLE_LOG("left closed")
    {
      a.close();
      auto w = a || b;
      reactor::wait(w);
    }
    ELLE_LOG("both closed")
    {
      b.close();
      bool beacon = false;
      auto w = a || b;
      reactor::run_later("open", [&] { BOOST_CHECK(!beacon); a.open(); });
      reactor::wait(w);
      beacon = true;
    }
    ELLE_LOG("right open")
    {
      auto w = a || b;
      reactor::wait(w);
    }
  }
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
  reactor::wait(waitables);
  ++step;
}

static
void
sender_one(int& step,
           reactor::Signal& s, int expect)
{
  BOOST_CHECK_EQUAL(step, 0);
  reactor::yield();
  BOOST_CHECK_EQUAL(step, 0);
  reactor::yield();
  BOOST_CHECK_EQUAL(step, 0);
  s.signal();
  BOOST_CHECK_EQUAL(step, 0);
  reactor::yield();
  reactor::yield();
  BOOST_CHECK_EQUAL(step, expect);
}

static
void
sender_two(int& step,
           reactor::Signal& s1,
           reactor::Signal& s2)
{
  BOOST_CHECK_EQUAL(step, 0);
  reactor::yield();
  BOOST_CHECK_EQUAL(step, 0);
  reactor::yield();
  BOOST_CHECK_EQUAL(step, 0);
  s1.signal();
  BOOST_CHECK_EQUAL(step, 0);
  reactor::yield();
  BOOST_CHECK_EQUAL(step, 0);
  reactor::yield();
  BOOST_CHECK_EQUAL(step, 0);
  reactor::yield();
  s2.signal();
  reactor::yield();
  reactor::yield();
  BOOST_CHECK_EQUAL(step, 1);
}

static
void
test_signals_one_on_one()
{
  reactor::Scheduler sched;

  int step = 0;
  reactor::Signal signal;
  reactor::Waitables signals;
  signals << signal;
  reactor::Thread w(sched, "waiter",
                    boost::bind(waiter, boost::ref(step), signals));
  reactor::Thread s(sched, "sender",
                    boost::bind(sender_one, boost::ref(step),
                                boost::ref(signal), 1));
  sched.run();
  BOOST_CHECK_EQUAL(step, 1);
}

static
void
test_signals_one_on_two()
{
  reactor::Scheduler sched;

  int step = 0;
  reactor::Signal signal1;
  reactor::Signal signal2;
  reactor::Waitables signals;
  signals << signal1 << signal2;
  reactor::Thread w(sched, "waiter",
                    boost::bind(waiter, boost::ref(step), signals));
  reactor::Thread s(sched, "sender",
                    boost::bind(sender_two, boost::ref(step),
                                boost::ref(signal1), boost::ref(signal2)));
  sched.run();
  BOOST_CHECK_EQUAL(step, 1);
}

static
void
test_signals_two_on_one()
{
  reactor::Scheduler sched;

  int step = 0;
  reactor::Signal signal;
  reactor::Waitables signals;
  signals << signal;
  reactor::Thread w1(sched, "waiter1",
                     boost::bind(waiter, boost::ref(step), signals));
  reactor::Thread w2(sched, "waiter2",
                     boost::bind(waiter, boost::ref(step), signals));
  reactor::Thread s(sched, "sender",
                    boost::bind(sender_one, boost::ref(step),
                                boost::ref(signal), 2));
  sched.run();
  BOOST_CHECK_EQUAL(step, 2);
}

ELLE_TEST_SCHEDULED(test_signals_order)
{
  // check that waiters are signaled in order.
  // try very hard to not pass by cheer luck
  int seq = -1;
  reactor::Signal s;
  reactor::Barrier b;
  auto thread = [&](int id) { b.open(); s.wait(); seq = id;};
  reactor::Thread t1("t1", std::bind(thread, 1));
  b.wait();b.close();
  reactor::Thread t2("t2", std::bind(thread, 2));
  b.wait();b.close();
  reactor::Thread t3("t3", std::bind(thread, 3));
  b.wait();b.close();
  for (unsigned int i = 4; i < 8; ++i)
  {
    new reactor::Thread(elle::sprintf("t%s", i), std::bind(thread, i), true);
    b.wait();b.close();
  }
  reactor::Thread t8("t8", std::bind(thread, 8));
  b.wait();b.close();
  reactor::Thread t9("t9", std::bind(thread, 9));
  b.wait();b.close();
  for (unsigned i=1; i<10; ++i)
  {
    BOOST_CHECK_EQUAL(seq, -1);
    s.signal_one();reactor::yield();reactor::yield();
    BOOST_CHECK_EQUAL(seq, i);
    seq = -1;
    reactor::yield();reactor::yield();
  }
  for (unsigned i=0; i<10; ++i)
    reactor::yield();
}

static
void
waiter_timeout()
{
  reactor::Signal s;
  bool finished = reactor::wait(s, boost::posix_time::milliseconds(10));
  BOOST_CHECK(!finished);
  s.signal();
}

static
void
test_signals_timeout()
{
  reactor::Scheduler sched;

  reactor::Thread t(sched, "waiter", waiter_timeout);
  sched.run();
}

/*--------.
| Barrier |
`--------*/

namespace barrier
{
  ELLE_TEST_SCHEDULED(closed)
  {
    reactor::Barrier barrier;
    bool beacon = false;
    reactor::Thread waiter("waiter", [&] {
        BOOST_CHECK(!barrier.opened());
        reactor::wait(barrier);
        BOOST_CHECK(barrier.opened());
        BOOST_CHECK(!beacon);
        beacon = true;
      });
    reactor::Thread opener("opener", [&] {
        reactor::yield();
        reactor::yield();
        reactor::yield();
        BOOST_CHECK(!beacon);
        barrier.open();
      });
    reactor::wait({waiter, opener});
    BOOST_CHECK(beacon);
  }

  ELLE_TEST_SCHEDULED(opened)
  {
    reactor::Barrier barrier;
    barrier.open();
    BOOST_CHECK(barrier.opened());
    reactor::Thread waiter("waiter", [&] {
        reactor::wait(barrier);
      });
    reactor::wait(waiter);
  }

  ELLE_TEST_SCHEDULED(inverted)
  {
    reactor::Barrier b;
    reactor::wait(!b);
    b.open();
    elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
    {
      reactor::Barrier timedout;
      scope.run_background(
        "closer",
        [&]
        {
          reactor::wait(timedout);
          b.close();
        });
      BOOST_CHECK(!reactor::wait(!b, 10_ms));
      timedout.open();
      reactor::wait(!b);
    };
  }

  ELLE_TEST_SCHEDULED(exception)
  {
    reactor::Barrier b;
    reactor::Thread waiter(
      "waiter",
      [&] {
        BOOST_CHECK(!b);
        BOOST_CHECK_THROW(reactor::wait(b), BeaconException);
        BOOST_CHECK(b);
        BOOST_CHECK_THROW(reactor::wait(b), BeaconException);
        BOOST_CHECK(b);
      });
    reactor::yield();
    reactor::yield();
    b.raise<BeaconException>();
    reactor::yield();
    reactor::yield();
    BOOST_CHECK_THROW(reactor::wait(b), BeaconException);
    b.open();
    reactor::wait(b);
    b.close();
    reactor::Thread rewaiter(
      "rewaiter",
      [&] {
        BOOST_CHECK(!b);
        reactor::wait(b);
      });
    reactor::yield();
    reactor::yield();
    b.open();
    reactor::wait(rewaiter);
  }
}

/*------------------.
| Multilock Barrier |
`------------------*/
static
void
multilock_barrier_basic()
{
  reactor::Scheduler sched;
  reactor::MultiLockBarrier barrier;
  bool no_lock = false;
  bool first_lock = false;
  bool second_lock = false;
  bool third_lock = false;
  bool beacon_waiter = false;
  bool beacon_closer = false;
  reactor::Thread waiter(sched, "waiter", [&] {
      BOOST_CHECK(barrier.opened());
      reactor::wait(barrier);
      no_lock = true;
      reactor::yield();
      reactor::yield();
      BOOST_CHECK_EQUAL(barrier.locks(), 1);
      BOOST_CHECK(first_lock);
      BOOST_CHECK(!second_lock);
      reactor::wait(barrier);
      BOOST_CHECK(barrier.opened());
      BOOST_CHECK(first_lock);
      BOOST_CHECK(second_lock);
      BOOST_CHECK(third_lock);
      beacon_waiter = true;
    });
  reactor::Thread closer(sched, "closer", [&] {
      reactor::yield();
      BOOST_CHECK(no_lock);
      {
        first_lock = true;
        auto lock = barrier.lock();
        reactor::yield();
        {
          second_lock = true;
          auto lock = barrier.lock();
          reactor::yield();
          {
            third_lock = true;
            auto copied_lock = lock;
            reactor::yield();
          }
        }
      }
      beacon_closer = true;
    });
  sched.run();
  BOOST_CHECK(beacon_waiter & beacon_closer);
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

  ELLE_TEST_SCHEDULED(exception, (bool, wait))
  {
    reactor::Scheduler sched;
    reactor::Thread t(
      sched, "main",
      [wait]
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
          if (wait)
            BOOST_CHECK_THROW(s.wait(), BeaconException);
          else
            BOOST_CHECK_THROW(reactor::sleep(), BeaconException);
          BOOST_CHECK(beacon);
        };
      });
    sched.run();
  }

  // Check we get past exceptions even if the scope is done.
  ELLE_TEST_SCHEDULED(exception_done)
  {
    elle::With<reactor::Scope>() << [&] (reactor::Scope& s)
    {
      auto& thread = s.run_background(
        "throw",
        [&]
        {
          throw BeaconException();
        });
      try
      {
        reactor::yield();
        reactor::yield();
      }
      catch (BeaconException const&)
      {}
      BOOST_CHECK(thread.done());
      BOOST_CHECK_THROW(s.wait(), BeaconException);
    };
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
          sched.terminate_now();
        };
      });
    sched.run();
  }

  // Insert a new thread while the Scope is being terminated. Used to delete the
  // new, still running thread.
  ELLE_TEST_SCHEDULED(terminate_insert)
  {
    reactor::Barrier leave;
    elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
    {
      scope.run_background(
        "1",
        [&]
        {
          reactor::sleep();
        });
      scope.run_background(
        "2",
        [&]
        {
          throw BeaconException();
        });
      reactor::yield();
      reactor::yield();
      BOOST_CHECK_THROW(
        scope.run_background(
          "3",
          [&]
          {
            reactor::sleep();
          }),
        BeaconException);
      BOOST_CHECK_THROW(reactor::wait(scope), BeaconException);
    };
  }
}

/*------.
| Sleep |
`------*/

static
void
test_sleep_interleave()
{
  reactor::Scheduler sched;
  int step = 0;
  reactor::Thread s1(
    sched, "sleeper1",
    [&]
    {
      BOOST_CHECK(step == 0 || step == 1);
      ++step;
      reactor::sleep(valgrind(200_ms, 5));
      BOOST_CHECK_EQUAL(step, 3);
      ++step;
    });
  reactor::Thread s2(
    sched, "sleeper2",
    [&]
    {
      BOOST_CHECK(step == 0 || step == 1);
      ++step;
      reactor::sleep(valgrind(100_ms, 5));
      BOOST_CHECK_EQUAL(step, 2);
      ++step;
    });
  sched.run();
}

static
boost::posix_time::ptime
now()
{
  return boost::posix_time::microsec_clock::local_time();
}

ELLE_TEST_SCHEDULED(test_sleep_timing)
{
  reactor::Duration const delay = valgrind(500_ms, 10);
  // The first sleep is erratic on valgrind, don't include it in the tests.
  if (RUNNING_ON_VALGRIND)
    reactor::sleep(delay);
  for (int i = 0; i < 4; ++i)
  {
    boost::posix_time::ptime start(now());
    reactor::sleep(delay);
    double elapsed = (now() - start).total_milliseconds();
    double expected =  delay.total_milliseconds();
    BOOST_CHECK_GE(elapsed, expected);
    BOOST_CHECK_CLOSE(elapsed, expected, double(25));
  }
}

/*------.
| Every |
`------*/

ELLE_TEST_SCHEDULED(every)
{
  reactor::Duration const delay = valgrind(200_ms, 10);
  static const int iter = 5;
  // The first sleep is erratic on valgrind, don't include it in the tests.
  if (RUNNING_ON_VALGRIND)
    reactor::sleep(delay);
  boost::posix_time::ptime start(now());
  int i = 0;
  reactor::Thread::unique_ptr thread = reactor::every(
    delay, "inc",
    [&]
    {
      if (++i >= iter)
        thread->terminate();
    });
  reactor::wait(*thread);
  BOOST_CHECK_EQUAL(i, iter);
  double elapsed = (now() - start).total_milliseconds();
  double expected =  delay.total_milliseconds() * iter;
  BOOST_CHECK_GE(elapsed, expected);
  reactor::sleep(delay * 3);
  BOOST_CHECK_EQUAL(i, iter);
}

/*-----.
| Join |
`-----*/

static
void
joined(int& count)
{
  BOOST_CHECK_EQUAL(count, 0);
  reactor::yield();
  ++count;
  reactor::yield();
  reactor::yield();
  ++count;
}

static
void
join_waiter(reactor::Thread& thread,
            int& count)
{
  reactor::wait(thread);
  BOOST_CHECK_EQUAL(count, 2);
  ++count;
}

static
void
test_join()
{
  reactor::Scheduler sched;

  int count = 0;
  reactor::Thread j(sched, "joined",
                    boost::bind(joined, boost::ref(count)));
  reactor::Thread w(sched, "waiter",
                    boost::bind(join_waiter,
                                boost::ref(j), boost::ref(count)));
  sched.run();
  BOOST_CHECK_EQUAL(count, 3);
}

static
void
join_waiter_multiple(reactor::Thread& thread,
                     int& count)
{
  reactor::yield();
  BOOST_CHECK(thread.state() == reactor::Thread::state::done);
  reactor::wait(thread);
  reactor::wait(thread);
  ++count;
}

static
void
test_join_multiple()
{
  reactor::Scheduler sched;

  int count = 0;
  reactor::Thread e(sched, "empty", [] {});
  reactor::Thread w(sched, "waiter",
                    boost::bind(join_waiter_multiple,
                                boost::ref(e), boost::ref(count)));
  sched.run();
  BOOST_CHECK_EQUAL(count, 1);
}

static
void
test_join_timeout()
{
  reactor::Scheduler sched;

  reactor::Thread sb(
    sched, "sleeping beauty",
    [&]
    {
      reactor::sleep(valgrind(200_ms, 10));
    });
  reactor::Thread pc(
    sched, "prince charming",
    [&]
    {
      bool finished = reactor::wait(sb, valgrind(100_ms, 10));
      BOOST_CHECK(!finished);
      BOOST_CHECK(!sb.done());
      finished = reactor::wait(sb, valgrind(200_ms, 10));
      BOOST_CHECK(finished);
      BOOST_CHECK(sb.done());
    });
  sched.run();
}

/*--------.
| Timeout |
`--------*/

static
void
timeout(reactor::Signal& s,
        bool expect)
{
  bool finished = reactor::wait(s, boost::posix_time::milliseconds(500));
  BOOST_CHECK(finished == expect);
  BOOST_CHECK(s.waiters().empty());
}

static
void
timeout_send(reactor::Signal& s)
{
  reactor::yield();
  BOOST_CHECK_EQUAL(s.waiters().size(), 1);
  s.signal();
}

static
void
test_timeout_do()
{
  reactor::Scheduler sched;

  reactor::Signal s;
  reactor::Thread t(sched, "timeout",
                    boost::bind(timeout, boost::ref(s), false));
  sched.run();
}

static
void
test_timeout_dont()
{
  reactor::Scheduler sched;

  reactor::Signal s;
  reactor::Thread t(sched, "timeout",
                    boost::bind(timeout, boost::ref(s), true));
  reactor::Thread p(sched, "poker",
                    boost::bind(timeout_send, boost::ref(s)));
  sched.run();
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
      reactor::wait(sem);
      throw BeaconException();
    });
  reactor::Thread waiter(sched, "waiter", [&] {
      sem.release();
      reactor::wait(thrower, 100_ms);
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
      reactor::wait(s, 11_ms);
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
  reactor::Scheduler sched;

  reactor::VThread<int> t(sched, "return value", answer);
  BOOST_CHECK_THROW(t.result(), elle::Exception);
  sched.run();
  BOOST_CHECK_EQUAL(t.result(), 42);
}

/*------------.
| Multithread |
`------------*/

static
void
test_multithread_spawn_wake()
{
  reactor::Scheduler sched;
  reactor::Barrier barrier;
  reactor::Thread keeper(sched, "keeper", [&] { reactor::wait(barrier); });
  std::thread s(
    [&]
    {
      ELLE_LOG("wait for the scheduler to be frozen")
        while (keeper.state() != reactor::Thread::state::frozen)
          ::usleep(10000);
      new reactor::Thread(sched, "waker", [&] { barrier.open(); }, true);
    });
  sched.run();
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
      int& res, reactor::Scheduler& sched)
{
  res = sched.mt_run<int>("spawned", boost::bind(spawned, boost::ref(s)));
}

static
void
spawner()
{
  reactor::Signal s;
  reactor::Scheduler& sched = *reactor::Scheduler::scheduler();
  int res = 0;
  boost::thread spawner(boost::bind(spawn, boost::ref(s), boost::ref(res), std::ref(sched)));
  reactor::wait(s);
  spawner.join();
  BOOST_CHECK_EQUAL(res, 42);
}

static
void
test_multithread_run()
{
  reactor::Scheduler sched;

  reactor::Thread t(sched, "spawner", spawner);
  sched.run();
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
  reactor::wait(s);
  BOOST_CHECK_EQUAL(s.count(), 1);
  reactor::wait(s);
  BOOST_CHECK_EQUAL(s.count(), 0);
}

static
void
test_semaphore_noblock()
{
  reactor::Scheduler sched;
  reactor::Semaphore s(2);
  reactor::Thread wait(sched, "wait",
                       boost::bind(&semaphore_noblock_wait, boost::ref(s)));
  sched.run();
}

static
void
semaphore_block_wait(reactor::Semaphore& s)
{
  BOOST_CHECK_EQUAL(s.count(), 0);
  reactor::wait(s);
  BOOST_CHECK_EQUAL(s.count(), 0);
}

static
void
semaphore_block_post(reactor::Semaphore& s)
{
  reactor::yield();
  reactor::yield();
  reactor::yield();
  BOOST_CHECK_EQUAL(s.count(), -1);
  s.release();
  BOOST_CHECK_EQUAL(s.count(), 0);
}

static
void
test_semaphore_block()
{
  reactor::Scheduler sched;
  reactor::Semaphore s;
  reactor::Thread wait(sched, "wait",
                       boost::bind(&semaphore_block_wait, boost::ref(s)));
  reactor::Thread post(sched, "post",
                       boost::bind(&semaphore_block_post, boost::ref(s)));
  sched.run();
}

static
void
test_semaphore_multi()
{
  reactor::Scheduler sched;
  reactor::Semaphore s;
  int step = 0;

  auto multi_wait = [&] { reactor::wait(s); ++step; };

  reactor::Thread wait1(sched, "wait1", multi_wait);
  reactor::Thread wait2(sched, "wait2", multi_wait);
  reactor::Thread post(sched, "post", [&] {
      reactor::yield();
      reactor::yield();
      reactor::yield();
      BOOST_CHECK_EQUAL(s.count(), -2);
      BOOST_CHECK_EQUAL(step, 0);
      s.release();
      reactor::yield();
      reactor::yield();
      BOOST_CHECK_EQUAL(s.count(), -1);
      BOOST_CHECK_EQUAL(step, 1);
      s.release();
      reactor::yield();
      reactor::yield();
      BOOST_CHECK_EQUAL(s.count(), 0);
      BOOST_CHECK_EQUAL(step, 2);
    });
  sched.run();
}

/*------.
| Mutex |
`------*/
namespace mutex
{

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
          reactor::yield();
        }
        ++i;
        prev = i;
      }
      reactor::yield();
    }
  }

  ELLE_TEST_SCHEDULED(basics)
  {
    reactor::Mutex mutex;
    int step = 0;
    reactor::Thread c1("counter1",
                       boost::bind(&mutex_count,
                                   boost::ref(step), boost::ref(mutex), 1));
    reactor::Thread c2("counter2",
                       boost::bind(&mutex_count,
                                   boost::ref(step), boost::ref(mutex), 1));
    reactor::Thread c3("counter3",
                       boost::bind(&mutex_count,
                                   boost::ref(step), boost::ref(mutex), 1));
    reactor::wait({c1, c2, c3});
  }
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
  reactor::yield();
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
  reactor::yield();
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
  reactor::yield();
  BOOST_CHECK_EQUAL(step, v);
  reactor::yield();
  BOOST_CHECK_EQUAL(step, v);
}

static
void
rw_mutex_both_write(reactor::RWMutex& mutex,
                    int& step)
{
  reactor::Lock lock(mutex.write());
  ++step;
  reactor::yield();
  reactor::yield();
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
  reactor::yield();
  BOOST_CHECK_EQUAL(val.Get(), start);
  val.Get()++;
  reactor::yield();
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

// Most likely a wine issue. To be investigated.
#ifndef INFINIT_WINDOWS
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
#endif

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
      reactor::yield();
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
  reactor::Scheduler sched;
  reactor::Thread thread(sched, "Exception Tester", &except_gen);
  bool exception_thrown = false;

  try
  {
    sched.run();
  }
  catch (const BeaconException& e)
  {
    BOOST_CHECK_EQUAL(e.what(), "beacon");
    exception_thrown = true;
  }

  BOOST_CHECK(exception_thrown);
  BOOST_CHECK_EQUAL(thread.state(), reactor::Thread::state::done);
}


static
void
exception_yield_pattern(std::vector<unsigned int> yield_pattern,
                        std::vector<bool> enable_pattern,
                        std::vector<unsigned int> no_exception)
{
  elle::With<reactor::Scope>() << [&] (reactor::Scope& s)
    {
      if (enable_pattern[0])
        s.run_background("t1", [&] {
            /* Workaround compiler bug when using captures in catch(...) block
             * Symptom: error: '...' handler must be the last handler for its
             * try block [-fpermissive]
            */
            unsigned yield_count = yield_pattern[0];
            try
            {
              throw std::runtime_error("t1");
            }
            catch(...)
            {
              for (unsigned i=0; i < yield_count; ++i)
                reactor::yield();
              BOOST_CHECK_EQUAL(elle::exception_string(), "t1");
            }
        });
      if (enable_pattern[1])
        s.run_background("t2", [&] {
            unsigned yield_count = yield_pattern[1];
            try
            {
              throw std::runtime_error("t2");
            }
            catch(...)
            {
              for (unsigned i=0; i < yield_count; ++i)
                reactor::yield();
              BOOST_CHECK_EQUAL(elle::exception_string(), "t2");
            }
        });
      if (enable_pattern[2])
        s.run_background("t3", [&] {
            unsigned yield_count = yield_pattern[2];
            try
            {
              throw std::runtime_error("t3");
            }
            catch(...)
            {
              for (unsigned i=0; i < yield_count; ++i)
                reactor::yield();
              try
              {
                throw;
              }
              catch(...)
              {
                BOOST_CHECK_EQUAL(elle::exception_string(), "t3");
              }
            }
        });
      if (enable_pattern[3])
        s.run_background("t4", [&] {
            unsigned yield_count = yield_pattern[3];
            try
            {
              try
              {
                throw std::runtime_error("t4");
              }
              catch(...)
              {
                for (unsigned i=0; i<yield_count; ++i)
                  reactor::yield();
                throw;
              }
            }
            catch(...)
            {
              BOOST_CHECK_EQUAL(elle::exception_string(), "t4");
            }
        });
      // check that current_exception stays empty on non-throwing threads
      auto no_exception_test = [&](unsigned int count) {
        if (!!std::current_exception())
            ELLE_ERR("Exception in no_exception thread: %s", elle::exception_string());
        BOOST_CHECK(!std::current_exception());
        for (unsigned i=0; i<count; ++i)
        {
          reactor::yield();
          if (!!std::current_exception())
            ELLE_ERR("Exception in no_exception thread: %s", elle::exception_string());
          BOOST_CHECK(!std::current_exception());
        }
      };
      for (unsigned i=0; i <no_exception.size(); ++i)
        s.run_background("tcheck",
          [&no_exception_test, &no_exception, i] {no_exception_test(no_exception[i]);});
      s.wait();
    };
}


ELLE_TEST_SCHEDULED(thread_exception_yield)
{
  // Check what happens when we yield in a catch block, and try to
  // reuse the exception later on.
  // Said differently, check that the semantic of std::current_exception is
  // not broken by yielding
  exception_yield_pattern({ 2, 1, 2, 1}, {true, true, true, true}, {1,3});
  exception_yield_pattern({ 2, 1, 2, 1}, {true, true, true, true}, {});
  exception_yield_pattern({ 1, 2, 3, 4}, {true, true, true, true}, {});
  exception_yield_pattern({ 4, 3, 2, 1}, {true, true, true, true}, {});
}

/*----------------.
| Terminate yield |
`----------------*/

static
void
test_terminate_yield()
{
  reactor::Scheduler sched;
  bool beacon = false;
  reactor::Thread t(
    sched, "thread",
    [&]
    {
      try
      {
        reactor::yield();
      }
      catch (reactor::Terminate const&)
      {
        reactor::yield();
        beacon = true;
        throw;
      }
      BOOST_ERROR("thread wasn't terminated");
    });
  reactor::Thread term(
    sched, "terminate",
    [&]
    {
      reactor::Scheduler& sched = *reactor::Scheduler::scheduler();
      sched.terminate();
    });
  sched.run();
  BOOST_CHECK(beacon);
}


/*--------------.
| Terminate now |
`--------------*/

ELLE_TEST_SCHEDULED(test_terminate_now)
{
  reactor::Barrier sleeping;
  bool beacon = false;
  reactor::Thread t(
    "terminated",
    [&]
    {
      beacon = false;
      try
      {
        while (true)
        {
          ELLE_LOG("wait for termination");
          sleeping.open();
          reactor::sleep();
        }
      }
      catch (...)
      {
        ELLE_LOG("delay termination");
        reactor::sleep(boost::posix_time::milliseconds(10));
        beacon = true;
        ELLE_LOG("actually die");
        throw;
      }
    });
  BOOST_CHECK_EQUAL(t.state(), reactor::Thread::state::running);
  reactor::wait(sleeping);
  t.terminate_now();
  BOOST_CHECK(beacon);
}

/*------------------------.
| Terminate now destroyed |
`------------------------*/

ELLE_TEST_SCHEDULED(test_terminate_now_destroyed)
{
  reactor::Barrier finish;
  reactor::Thread t1(
    "t1",
    [&]
    {
      // Ignore the first Terminate (sent by t2) and catch the second one (by
      // t4).
      try
      {
        ELLE_LOG("sleep")
          reactor::sleep();
      }
      catch (...)
      {
        ELLE_LOG("block until test end")
          finish.wait();
      }
    });
  reactor::Thread t2(
    "t2",
    [&]
    {
      ELLE_LOG("terminate thread 1")
        t1.terminate_now();
      BOOST_ERROR("terminate_now should have failed (killed by t3 during wait)");
    });
  reactor::Thread t3(
    "t3",
    [&]
    {
      ELLE_LOG("terminate thread 2")
        t2.terminate_now();
    });
  while (true)
  {
    // If t2 is effectively waiting for t1 to die and t3 for t2 to die
    // then the test has passed.
    if (t3.state() == reactor::Thread::state::frozen)
    {
      finish.open();
      break;
    }
    BOOST_CHECK(!t1.done());
    BOOST_CHECK(!t2.done());
    reactor::yield();
  }
  reactor::wait(t3);
}

/*-----------------------.
| Terminate now disposed |
`-----------------------*/

static
void
terminated(bool& terminated)
{

  terminated = false;
  try
  {
    while (true)
    {
      reactor::yield();
    }
  }
  catch (...)
  {
    terminated = true;
    reactor::sleep(boost::posix_time::milliseconds(10));
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

ELLE_TEST_SCHEDULED(test_terminate_now_starting_dispose)
{
  auto t = new reactor::Thread("starting", [] {}, true);
  t->terminate_now();
}

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

/*----------------------.
| Terminate now started |
`----------------------*/

static
void
test_terminate_now_started()
{
  bool beacon = true;
  reactor::Scheduler sched;
  std::unique_ptr<reactor::Thread> starting;
  reactor::Thread terminate(sched, "terminate",
                            [&] { starting->terminate_now(); });
  starting.reset(
    new reactor::Thread(sched, "starting",
                        [&] { beacon = false; reactor::yield(); }));
  sched.run();
  BOOST_CHECK(beacon);
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

  ++beacon;
  try
  {
    reactor::yield();
  }
  catch (...)
  {
    ++beacon;
    reactor::yield();
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

// Check thread are not re-terminated.
ELLE_TEST_SCHEDULED(test_terminate_twice)
{
  reactor::Barrier waiting;
  reactor::Barrier reterminated;
  reactor::Barrier rewaiting;
  reactor::Thread thread(
    "waiter",
    [&]
    {
      try
      {
        waiting.open();
        reactor::sleep();
      }
      catch (reactor::Terminate const&)
      {
        rewaiting.open();
        BOOST_CHECK_NO_THROW(reactor::wait(reterminated));
        throw;
      }
    });
  reactor::wait(waiting);
  thread.terminate();
  reactor::wait(rewaiting);
  thread.terminate();
  reterminated.open();
  reactor::wait(thread);
}

// Check exception swallowing mechanism false positives.
ELLE_TEST_SCHEDULED(test_terminate_not_swallowed_unwinding)
{
  bool beacon = false;
  reactor::Barrier waiting;
  class Waiter
  {
  public:
    Waiter(bool& beacon)
      : _beacon(beacon)
    {}

    ~Waiter()
    {
      reactor::yield();
      this->_beacon = true;
    }
  private:
    bool& _beacon;
  };
  reactor::Thread thread(
    "survivor",
    [&]
    {
      Waiter waiter(beacon);
      waiting.open();
      reactor::sleep();
    });
  reactor::wait(waiting);
  thread.terminate_now();
  BOOST_CHECK(beacon);
}

// Check exception swallowing mechanism false positives.
ELLE_TEST_SCHEDULED(test_terminate_not_swallowed_catch)
{
  bool beacon = false;
  reactor::Barrier waiting;
  reactor::Thread thread(
    "survivor",
    [&]
    {
      waiting.open();
      try
      {
        reactor::sleep();
      }
      catch (reactor::Terminate const&)
      {
        reactor::yield();
        beacon = true;
      }
    });
  reactor::wait(waiting);
  thread.terminate_now();
  BOOST_CHECK(beacon);
}

// Check you can't swallow terminate exception.
// ELLE_TEST_SCHEDULED(test_terminate_swallowed)
// {
//   int i = 0;
//   int const n = 8;
//   reactor::Barrier waiting;
//   reactor::Thread thread(
//     "survivor",
//     [&]
//     {
//       while (++i < n)
//       {
//         try
//         {
//           waiting.open();
//           reactor::sleep();
//         }
//         catch (reactor::Terminate const&)
//         {}
//       }
//     });
//   reactor::wait(waiting);
//   thread.terminate_now();
//   BOOST_CHECK_EQUAL(i, n);
// }

/*-----------------.
| IO service throw |
`-----------------*/

static
void
poster(bool& beacon)
{
  auto& sched = *reactor::Scheduler::scheduler();
  sched.io_service().post(&except_gen);
  reactor::yield();
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
  ELLE_TEST_SCHEDULED(operation)
  {
    std::mutex mutex;
    std::condition_variable cv;
    bool backgrounded = false;
    int i = 0;
    reactor::Thread counter(
      "counter",
      [&]
      {
        do
        {
          std::unique_lock<std::mutex> lock(mutex);
          if (backgrounded)
            break;
          reactor::yield();
        }
        while (true);
        ++i;
        reactor::yield();
        ++i;
        reactor::yield();
        ++i;
        BOOST_CHECK(backgrounded);
        std::unique_lock<std::mutex> lock(mutex);
        cv.notify_one();
      });
    reactor::background([&]
                        {
                          std::unique_lock<std::mutex> lock(mutex);
                          backgrounded = true;
                          cv.wait(lock);
                          backgrounded = false;
                        });
    BOOST_CHECK_EQUAL(i, 3);
  }

  ELLE_TEST_SCHEDULED(operations)
  {
    static int const iterations = 16;
    reactor::Duration sleep_time = valgrind(500_ms, 5);
    // The first sleep is erratic on valgrind, don't include it in the
    // tests.
    if (RUNNING_ON_VALGRIND)
      reactor::sleep(sleep_time);
    // Run it three times to check the thread pool doesn't exceed 16.
    for (int run = 0; run < 3; ++run)
    {
      int count = 0;
      std::vector<reactor::Thread*> threads;
      for (int i = 0; i < iterations; ++i)
        threads.push_back(
          new reactor::Thread(
            elle::sprintf("thread %s", i),
            [&count, &sleep_time]
            {
              reactor::background(
                [&]
                {
                  ::usleep(sleep_time.total_microseconds());
                });
              ++count;
            }));
      auto start = boost::posix_time::microsec_clock::local_time();
      reactor::wait(reactor::Waitables(begin(threads), end(threads)));
      auto duration =
        boost::posix_time::microsec_clock::local_time() - start;
      BOOST_CHECK_EQUAL(count, iterations);
      BOOST_CHECK_EQUAL(
        reactor::scheduler().background_pool_size(), iterations);
      BOOST_CHECK_LT(duration, sleep_time * 3);
      for (auto thread: threads)
        delete thread;
    }
  }

  ELLE_TEST_SCHEDULED(exception)
  {
    BOOST_CHECK_THROW(reactor::background([] { throw BeaconException(); }),
                      std::exception);
    BOOST_CHECK_THROW(reactor::background([] { throw BeaconException(); }),
                      std::exception);
    BOOST_CHECK_THROW(reactor::background([] { throw BeaconException(); }),
                      std::exception);
    BOOST_CHECK_EQUAL(reactor::scheduler().background_pool_size(), 1);
  }

  ELLE_TEST_SCHEDULED(aborted)
  {
    reactor::Thread main(
      "main",
      [&]
      {
        auto done = std::make_shared<bool>(false);
        auto const sleep_time = 500_ms;
        try
        {
          reactor::background([done, sleep_time]
                              {
                                ::usleep(sleep_time.total_microseconds());
                                *done = true;
                              });
        }
        catch (reactor::Terminate const&)
        {
          BOOST_CHECK(!*done);
          return;
        }
        BOOST_ERROR("background task was not terminated");
      });
    reactor::Thread kill(
      "kill",
      [&]
      {
        main.terminate();
      });
    reactor::wait(main);
  }

  ELLE_TEST_SCHEDULED(aborted_throw)
  {
    reactor::Barrier backgrounded;
    bool terminated = false;
    std::mutex mtx;
    std::condition_variable barrier;
    reactor::Thread background(
      "background",
      [&]
      {
        backgrounded.open();
        reactor::background(
          [&]
          {
            // Wait for the "background" reactor::Thread to be terminated.
            {
              std::unique_lock<std::mutex> lock(mtx);
              if (!terminated)
                barrier.wait(lock);
              BOOST_CHECK(terminated);
              barrier.notify_all();
              throw BeaconException();
            }
          });
        BOOST_ERROR("should have been aborted");
      });
    reactor::wait(backgrounded);
    {
      std::unique_lock<std::mutex> lock(mtx);
      background.terminate_now();
      barrier.notify_all();
      terminated = true;
      barrier.wait(lock);
    }
  }

  ELLE_TEST_SCHEDULED(future)
  {
    ELLE_LOG("test plain value")
    {
      reactor::BackgroundFuture<int> f(42);
      BOOST_CHECK_EQUAL(f.value(), 42);
    }
    ELLE_LOG("test waiting value")
    {
      reactor::BackgroundFuture<int> f([] { ::usleep(100000); return 51; });
      BOOST_CHECK_EQUAL(f.value(), 51);
      f = [] { ::usleep(100000); return 69; };
      BOOST_CHECK_EQUAL(f.value(), 69);
    }
    ELLE_LOG("test already available value")
    {
      reactor::BackgroundFuture<int> f([] { return 69; });
      reactor::sleep(200_ms);
      BOOST_CHECK_EQUAL(f.value(), 69);
    }
    ELLE_LOG("test killing unfinished")
    {
      reactor::Barrier destroying;
      bool sleeping = false;
      bool over = false;
      std::condition_variable sync;
      auto mtx = std::make_shared<std::mutex>();
      reactor::Thread t(
        "future",
        [&, mtx]
        {
          reactor::BackgroundFuture<int> f(
            [&, mtx]
            {
              {
                std::unique_lock<std::mutex> lock(*mtx);
                sleeping = true;
                sync.notify_one();
              }
              {
                std::unique_lock<std::mutex> lock(*mtx);
                while (!over)
                  sync.wait(lock);
              }
              return 42;
            });
          {
            std::unique_lock<std::mutex> lock(*mtx);
            while (!sleeping)
              sync.wait(lock);
          }
          destroying.open();
        });
      reactor::wait(destroying);
      t.terminate_now();
      {
        std::unique_lock<std::mutex> lock(*mtx);
        over = true;
        sync.notify_one();
      }
    }
  }
}

/*--------.
| Signals |
`--------*/

#if !defined(INFINIT_WINDOWS) && !defined(INFINIT_IOS)
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
                        reactor::wait(t);
                      });
    sched.signal_handle(SIGINT, [&sched] { sched.terminate(); });
    sched.run();
  }
}
#endif

ELLE_TEST_SCHEDULED(test_simple_channel)
{

  reactor::Channel<int> channel;

  elle::With<reactor::Scope>() << [&](reactor::Scope &s)
  {
    s.run_background("producer", [&]() { channel.put(42); });

    s.run_background("consumer", [&]() {
               int final = channel.get();
               BOOST_CHECK_EQUAL(final, 42);
            });

    reactor::wait(s);
  };
}

ELLE_TEST_SCHEDULED(test_multiple_channel)
{
  reactor::Channel<int> channel;
  reactor::Barrier reading;

  elle::With<reactor::Scope>() << [&](reactor::Scope &s)
  {
    std::list<int> list ({1, 2, 3, 4, 5});
    s.run_background("consumer",
                     [&]
                     {
                       reading.open();
                       for (int i: list)
                         BOOST_CHECK_EQUAL(channel.get(), i);
                     });
    s.run_background("producer",
                     [&]()
                     {
                       reactor::wait(reading);
                       for (int i: list)
                         channel.put(i);
                     });
   reactor::wait(s);
  };
}


ELLE_TEST_SCHEDULED(test_multiple_consumers)
{
  reactor::Channel<int> channel;
  bool got = false;
  elle::With<reactor::Scope>() << [&](reactor::Scope &s)
  {
    auto action = [&]
      {
        BOOST_CHECK_EQUAL(channel.get(), 42);
        if (!got)
        {
          got = true;
          reactor::yield();
          reactor::yield();
          reactor::Scheduler::scheduler()->terminate();
        }
        else
          BOOST_FAIL("value was read twice");
      };
    s.run_background("consumer 1", action);
    s.run_background("consumer 2", action);
    s.run_background("producer", [&] { channel.put(42); });
    reactor::wait(s);
  };
}

namespace channel
{
  ELLE_TEST_SCHEDULED(wake_clear)
  {
    reactor::Channel<int> channel;
    elle::With<reactor::Scope>() << [&](reactor::Scope &s)
    {
      s.run_background(
        "reader",
        [&]
        {
          BOOST_CHECK_EQUAL(channel.get(), 51);
        });
      s.run_background(
        "writer",
        [&]
        {
          channel.put(42);
          channel.clear();
          reactor::yield();
          reactor::yield();
          channel.put(51);
        });
    reactor::wait(s);
    };
  }

  ELLE_TEST_SCHEDULED(open_close)
  {
    reactor::Channel<int> channel;
    reactor::Signal gotcha;
    elle::With<reactor::Scope>() << [&](reactor::Scope &s)
    {
      s.run_background(
        "reader",
        [&]
        {
          BOOST_CHECK_EQUAL(channel.get(), 0);
          gotcha.signal();
          BOOST_CHECK_EQUAL(channel.get(), 1);
          gotcha.signal();
          BOOST_CHECK_EQUAL(channel.get(), 2);
          gotcha.signal();
        });
      s.run_background(
        "writer",
        [&]
        {
          channel.put(0);
          reactor::wait(gotcha);
          channel.close();
          channel.put(1);
          BOOST_CHECK(!reactor::wait(gotcha, 500_ms));
          channel.open();
          channel.put(2);
          reactor::wait(gotcha);
        });
    reactor::wait(s);
    };
  }
}

ELLE_TEST_SCHEDULED(test_released_signal)
{
  using reactor::Thread;
  {
    bool beacon = false;
    Thread t("test 1", [&] { reactor::yield(); });
    t.released().connect([&] { beacon = true; });
    BOOST_CHECK(!beacon);
    reactor::wait(t);
    BOOST_CHECK(beacon);
  }
  // terminate_now from thread
  {
    bool beacon = false;
    Thread t("test 2", [&] { t.terminate_now(true); });
    t.released().connect([&] { beacon = true; });
    BOOST_CHECK(!beacon);
    reactor::wait(t);
    BOOST_CHECK(beacon);
  }
  // terminate_now
  {
    bool beacon = false;
    Thread t("test 3", [&] { reactor::sleep(); });
    t.released().connect([&] { beacon = true; });
    BOOST_CHECK(!beacon);
    t.terminate_now();
    BOOST_CHECK(beacon);
  }
}

namespace tracked
{
  ELLE_TEST_SCHEDULED(reset_before)
  {
    auto t = reactor::Thread::make_tracked("test", [] {});
    t.reset();
    reactor::yield();
    reactor::yield();
    reactor::yield();
  }

  ELLE_TEST_SCHEDULED(reset_after)
  {
    auto t = reactor::Thread::make_tracked("test", [] {});
    reactor::yield();
    reactor::yield();
    reactor::yield();
    t.reset();
  }
}

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

namespace timer
{
  using reactor::Timer;

  ELLE_TEST_SCHEDULED(wait)
  {
    int v = 0;
    Timer t("myTimer1", 200_ms, std::bind(&coro, std::ref(v)));
    BOOST_CHECK_EQUAL(v, 0);
    t.wait();
    BOOST_CHECK_EQUAL(v, 2);
  }

  ELLE_TEST_SCHEDULED(destructor)
  {
    bool v = false;
    reactor::Barrier started;
    {
      Timer t("myTimer2", 0_ms,
              [&]
              {
                started.open();
                reactor::yield();
                reactor::yield();
                reactor::yield();
                reactor::yield();
                v = true;
              });
      reactor::wait(started);
    }
    BOOST_CHECK(v);
  }

  ELLE_TEST_SCHEDULED(basic_cancel)
  {
    int v = 0;
    Timer t("myTimer3", 100_ms, std::bind(&coro, std::ref(v)));
    BOOST_CHECK_EQUAL(v, 0);
    t.cancel();
    reactor::sleep(200_ms);
    BOOST_CHECK_EQUAL(v, 0);
  }

  ELLE_TEST_SCHEDULED(cancel_after_start)
  {
    reactor::Barrier b;
    reactor::Barrier b2;
    int v = 0;
    Timer t("myTimer4", 0_ms, [&] { b.open(); v = 1; b2.wait(); v=2;});
    b.wait();
    BOOST_CHECK_EQUAL(v, 1);
    t.cancel();
    b2.open();
    reactor::yield();
    reactor::yield();
    BOOST_CHECK_EQUAL(v, 2);
  }

  ELLE_TEST_SCHEDULED(cancel_now_after_start)
  {
    reactor::Barrier b;
    int v = 0;
    Timer t("myTimer5", 0_ms, [&] { b.open(); v = 1; reactor::yield(); reactor::yield(); v=2;});
    b.wait();
    t.cancel_now(); // Waits.
    BOOST_CHECK_EQUAL(v, 2);
  }

  ELLE_TEST_SCHEDULED(terminate_after_start)
  {
    int v = 0;
    reactor::Barrier b;
    Timer t("myTimer6", 0_ms, [&]
            {
              try {
                b.open(); v = 1; reactor::yield(); reactor::yield(); v=2;
              }
              catch (...)
              {
                // Check we were interrupted.
                v = 3;
                throw;
              }
            });
    b.wait();
    t.terminate();
    BOOST_CHECK_EQUAL(v, 1);
    reactor::yield(); reactor::yield();
    BOOST_CHECK_EQUAL(v, 3);
  }

  ELLE_TEST_SCHEDULED(terminate_now_after_start)
  {
    int v = 0;
    reactor::Barrier b;
    Timer t("myTimer7", 0_ms, [&]
            {
              try {
                b.open(); v = 1; reactor::yield(); reactor::yield(); v=2;
              }
              catch (...)
              {
                v = 3;
                throw;
              }
            });
    b.wait();
    t.terminate_now();
    BOOST_CHECK_EQUAL(v, 3);
  }
}

namespace timeout_
{
  ELLE_TEST_SCHEDULED(timeout)
  {
    elle::With<reactor::Scope>() << [] (reactor::Scope& scope)
    {
      bool beacon1 = false;
      bool beacon2 = false;
      scope.run_background(
        "control",
        [&]
        {
          reactor::sleep(valgrind(50_ms, 20));
          beacon1 = true;
          reactor::sleep(valgrind(100_ms, 20));
          beacon2 = true;
        });
      try
      {
        reactor::TimeoutGuard timeout(valgrind(100_ms, 20));
        reactor::sleep(valgrind(200_ms, 20));
        BOOST_ERROR("didn't timeout");
      }
      catch(reactor::Timeout const&)
      {}
      BOOST_CHECK(beacon1);
      BOOST_CHECK(!beacon2);
    };
  }

  ELLE_TEST_SCHEDULED(complete)
  {
    elle::With<reactor::Scope>() << [] (reactor::Scope& scope)
    {
      try
      {
        reactor::TimeoutGuard timeout(500_ms);
        reactor::sleep(100_ms);
      }
      catch(reactor::Timeout const&)
      {
        BOOST_ERROR("timeout");
      }
    };
  }

  ELLE_TEST_SCHEDULED(race_condition)
  {
    try
    {
      auto const sleep_time = valgrind(10_ms);
      reactor::TimeoutGuard timeout(sleep_time);
      ::usleep((sleep_time * 2).total_microseconds());
    }
    catch(reactor::Timeout const&)
    {
      BOOST_ERROR("timeout");
    }
  }
}

namespace non_interruptible
{
  ELLE_TEST_SCHEDULED(terminate)
  {
    ELLE_LOG("test 1");
    {
      reactor::Barrier b,c;
      int status = 0;
      reactor::Thread t(
        "test",
        [&]
        {
          elle::With<reactor::Thread::NonInterruptible>() << [&]
          {
            status = 1;
            b.open();
            reactor::wait(c);
            status = 2;
          };
        });
      reactor::wait(b);
      b.close(); c.open();
      t.terminate_now();
      BOOST_CHECK_EQUAL(status, 2);
    }
    ELLE_LOG("test 2");
    {
      reactor::Barrier b,c,d;
      int status = 0;
      reactor::Thread t(
        "test",
        [&]
        {
          elle::With<reactor::Thread::NonInterruptible>() << [&]
          {
            status = 1;
            b.open();
            reactor::wait(c);
            status = 2;
          };
          reactor::wait(d);
          status = 3;
        });
      reactor::wait(b);
      c.open();
      reactor::yield();
      t.terminate_now();
      BOOST_CHECK_EQUAL(status, 2);
    }
    ELLE_LOG("test 3")
    {
      reactor::Scheduler sched;
      reactor::Barrier ready("ready"), go("go"), terminated("terminated");
      reactor::Thread t(
        sched, "main",
        [&]
        {
          elle::With<reactor::Scope>() << [&] (reactor::Scope& s)
          {
            auto& t1 = s.run_background(
              "1",
              [&]
              {
                try
                {
                  ELLE_TRACE("open %s", ready)
                    ready.open();
                  {
                    elle::With<reactor::Thread::NonInterruptible>() << [&]
                    {
                      // Because it's not interruptible, reactor should wait for
                      // ever...
                      ELLE_TRACE("wait for %s", go)
                        go.wait();
                      ELLE_TRACE("go status: %s", (bool) go);
                    };
                  }
                }
                catch (reactor::Terminate const&)
                {
                  BOOST_CHECK(ready);
                  BOOST_CHECK(go);
                  ELLE_TRACE("open %s", terminated)
                    terminated.open();
                  throw;
                }
              });
            auto& t2 = s.run_background(
              "2",
              [&]
              {
                ELLE_TRACE("wait for %s", ready)
                  ready.wait();
                reactor::yield();
                // Terminate shoulb be blocked until go is open.
                ELLE_TRACE("terminate %s", t1);
                  t1.terminate_now();
                BOOST_CHECK(go);
                BOOST_CHECK(terminated);
              });
            s.run_background(
              "3",
              [&]
              {
                ELLE_TRACE("wait for %s", ready)
                  ready.wait();
                reactor::yield();
                reactor::yield();
                BOOST_CHECK(!t1.done());
                BOOST_CHECK(!t2.done());
                ELLE_TRACE("open %s", go)
                  go.open();
                BOOST_CHECK(!t1.done());
                BOOST_CHECK(!t2.done());
                ELLE_TRACE("wait fo %s", terminated)
                  terminated.wait(1_sec);
                reactor::yield();
                reactor::yield();
                BOOST_CHECK(t1.done());
                BOOST_CHECK(t2.done());
              });
            s.wait();
          };
        });
      sched.run();
    }
  }

  ELLE_TEST_SCHEDULED(nested_exception)
  {
    bool beacon = false;
    reactor::Barrier sleeping;
    reactor::Channel<int> poke;
    reactor::Thread cobaye(
      "cobaye",
      [&]
      {
        try
        {
          elle::With<reactor::Thread::NonInterruptible>() << [&]
          {
            sleeping.open();
            poke.get();
            throw BeaconException();
          };
        }
        catch (BeaconException const&)
        {}
        catch (reactor::Terminate const&)
        {
          beacon = true;
        }
      });
    reactor::wait(sleeping);
    poke.put(42);
    cobaye.terminate_now();
    BOOST_CHECK(beacon);
  }
}

/*---------.
| For each |
`---------*/

namespace for_each
{
  ELLE_TEST_SCHEDULED(parallel)
  {
    std::vector<int> c{0, 1, 2};
    reactor::Barrier b;
    reactor::Thread check(
      "check",
      [&]
      {
        BOOST_CHECK_EQUAL(c, std::vector<int>({0, 1, 2}));
        reactor::yield();
        BOOST_CHECK_EQUAL(c, std::vector<int>({1, 2, 3}));
        reactor::yield();
        b.open();
      });
    reactor::for_each_parallel(
      c,
      [&] (int& c)
      {
        ++c;
        reactor::wait(b);
      });
    BOOST_CHECK(b.opened());
  }

  ELLE_TEST_SCHEDULED(parallel_break)
  {
    std::vector<int> c{0, 1, 2};
    reactor::for_each_parallel(
      c,
      [&] (int& c)
      {
        if (c == 1)
          reactor::break_parallel();
        ++c;
      });
    BOOST_CHECK_EQUAL(c, std::vector<int>({1, 1, 2}));
  }
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  {
    boost::unit_test::test_suite* basics = BOOST_TEST_SUITE("basics");
    boost::unit_test::framework::master_test_suite().add(basics);
    basics->add(BOOST_TEST_CASE(test_basics_one), 0, valgrind(1, 5));
    basics->add(BOOST_TEST_CASE(test_basics_interleave), 0, valgrind(1, 5));
    basics->add(BOOST_TEST_CASE(nested_schedulers), 0, valgrind(1, 5));
    basics->add(BOOST_TEST_CASE(managed), 0, valgrind(1, 5));
    basics->add(BOOST_TEST_CASE(non_managed), 0, valgrind(1, 5));
    basics->add(BOOST_TEST_CASE(unique_ptr), 0, valgrind(1, 5));
    basics->add(BOOST_TEST_CASE(deadlock), 0, valgrind(1, 5));
  }

  {
    boost::unit_test::test_suite* channels = BOOST_TEST_SUITE("channel");
    boost::unit_test::framework::master_test_suite().add(channels);
    channels->add(BOOST_TEST_CASE(test_simple_channel), 0, valgrind(1, 5));
    channels->add(BOOST_TEST_CASE(test_multiple_channel), 0, valgrind(1, 5));
    channels->add(BOOST_TEST_CASE(test_multiple_consumers), 0, valgrind(1, 5));
    auto wake_clear = &channel::wake_clear;
    channels->add(BOOST_TEST_CASE(wake_clear), 0, valgrind(1, 5));
    auto open_close = &channel::open_close;
    channels->add(BOOST_TEST_CASE(open_close), 0, valgrind(1, 5));
  }

  {
    boost::unit_test::test_suite* subsuite = BOOST_TEST_SUITE("waitable");
    boost::unit_test::framework::master_test_suite().add(subsuite);
    using namespace waitable;
    subsuite->add(BOOST_TEST_CASE(exception_no_wait), 0, valgrind(1, 5));
    subsuite->add(BOOST_TEST_CASE(logical_or), 0, valgrind(1, 5));
  }

  boost::unit_test::test_suite* signals = BOOST_TEST_SUITE("Signals");
  boost::unit_test::framework::master_test_suite().add(signals);
  signals->add(BOOST_TEST_CASE(test_signals_one_on_one), 0, valgrind(1, 5));
  signals->add(BOOST_TEST_CASE(test_signals_one_on_two), 0, valgrind(1, 5));
  signals->add(BOOST_TEST_CASE(test_signals_two_on_one), 0, valgrind(1, 5));
  signals->add(BOOST_TEST_CASE(test_signals_timeout), 0, valgrind(1, 5));
  signals->add(BOOST_TEST_CASE(test_signals_order), 0, valgrind(1, 5));

  {
    auto barrier = BOOST_TEST_SUITE("barrier");
    boost::unit_test::framework::master_test_suite().add(barrier);
    using namespace barrier;
    barrier->add(BOOST_TEST_CASE(closed), 0, valgrind(1, 5));
    barrier->add(BOOST_TEST_CASE(opened), 0, valgrind(1, 5));
    barrier->add(BOOST_TEST_CASE(inverted), 0, valgrind(1, 5));
    barrier->add(BOOST_TEST_CASE(exception), 0, valgrind(1, 5));
  }

  boost::unit_test::test_suite* multilock_barrier =
    BOOST_TEST_SUITE("MultilockBarrier");
  boost::unit_test::framework::master_test_suite().add(multilock_barrier);
  multilock_barrier->add(BOOST_TEST_CASE(multilock_barrier_basic), 0, valgrind(1, 5));

  // Timer
  {
    boost::unit_test::test_suite* timer = BOOST_TEST_SUITE("timer");
    boost::unit_test::framework::master_test_suite().add(timer);
    auto wait = &timer::wait;
    timer->add(BOOST_TEST_CASE(wait), 0, valgrind(1, 5));
    auto destructor = &timer::destructor;
    timer->add(BOOST_TEST_CASE(destructor), 0, valgrind(1, 5));
    auto basic_cancel = &timer::basic_cancel;
    timer->add(BOOST_TEST_CASE(basic_cancel), 0, valgrind(1, 5));
    auto cancel_after_start = &timer::cancel_after_start;
    timer->add(BOOST_TEST_CASE(cancel_after_start), 0, valgrind(1, 5));
    auto cancel_now_after_start = &timer::cancel_now_after_start;
    timer->add(BOOST_TEST_CASE(cancel_now_after_start), 0, valgrind(1, 5));
    auto terminate_after_start = &timer::terminate_after_start;
    timer->add(BOOST_TEST_CASE(terminate_after_start), 0, valgrind(1, 5));
    auto terminate_now_after_start = &timer::terminate_now_after_start;
    timer->add(BOOST_TEST_CASE(terminate_now_after_start), 0, valgrind(1, 5));
  }

  // Scope
  {
    boost::unit_test::test_suite* scope = BOOST_TEST_SUITE("scope");
    boost::unit_test::framework::master_test_suite().add(scope);
    auto empty = &scope::empty;
    scope->add(BOOST_TEST_CASE(empty), 0, valgrind(1, 5));
    auto wait = &scope::wait;
    scope->add(BOOST_TEST_CASE(wait), 0, valgrind(1, 5));
    auto exception_wait = std::bind(&scope::exception, true);
    scope->add(BOOST_TEST_CASE(exception_wait), 0, valgrind(1, 5));
    auto exception_sleep = std::bind(&scope::exception, false);
    scope->add(BOOST_TEST_CASE(exception_sleep), 0, valgrind(1, 5));
    auto exception_done = &scope::exception_done;
    scope->add(BOOST_TEST_CASE(exception_done), 0, valgrind(1, 5));
    auto multiple_exception = &scope::multiple_exception;
    scope->add(BOOST_TEST_CASE(multiple_exception), 0, valgrind(1, 5));
    auto terminate = &scope::terminate;
    scope->add(BOOST_TEST_CASE(terminate), 0, valgrind(1, 5));
    auto terminate_all = &scope::terminate_all;
    scope->add(BOOST_TEST_CASE(terminate_all), 0, valgrind(1, 5));
    auto terminate_insert = &scope::terminate_insert;
    scope->add(BOOST_TEST_CASE(terminate_insert), 0, valgrind(1, 5));
  }

  {
    boost::unit_test::test_suite* sleep = BOOST_TEST_SUITE("sleep");
    boost::unit_test::framework::master_test_suite().add(sleep);
    sleep->add(BOOST_TEST_CASE(test_sleep_interleave), 0, valgrind(1, 5));
    sleep->add(BOOST_TEST_CASE(test_sleep_timing), 0, valgrind(10, 3));
  }

  {
    boost::unit_test::framework::master_test_suite().add(
      BOOST_TEST_CASE(every), 0, valgrind(2));
  }

  boost::unit_test::test_suite* join = BOOST_TEST_SUITE("Join");
  boost::unit_test::framework::master_test_suite().add(join);
  join->add(BOOST_TEST_CASE(test_join), 0, valgrind(1, 5));
  join->add(BOOST_TEST_CASE(test_join_multiple), 0, valgrind(1, 5));
  join->add(BOOST_TEST_CASE(test_join_timeout), 0, valgrind(1, 5));

  boost::unit_test::test_suite* terminate = BOOST_TEST_SUITE("terminate");
  boost::unit_test::framework::master_test_suite().add(terminate);
  terminate->add(BOOST_TEST_CASE(test_terminate_yield), 0, valgrind(1, 5));
  terminate->add(BOOST_TEST_CASE(test_terminate_now), 0, valgrind(1, 5));
  terminate->add(BOOST_TEST_CASE(test_terminate_now_destroyed), 0, valgrind(1, 5));
  terminate->add(BOOST_TEST_CASE(test_terminate_now_disposed), 0, valgrind(1, 5));
  terminate->add(BOOST_TEST_CASE(test_terminate_now_starting), 0, valgrind(1, 5));
  terminate->add(BOOST_TEST_CASE(test_terminate_now_starting_dispose), 0, valgrind(1, 5));
  terminate->add(BOOST_TEST_CASE(test_terminate_now_started), 0, valgrind(1, 5));
  terminate->add(BOOST_TEST_CASE(test_terminate_now_scheduled), 0, valgrind(1, 5));
  terminate->add(BOOST_TEST_CASE(test_exception_escape), 0, valgrind(1, 5));
  terminate->add(BOOST_TEST_CASE(test_exception_escape_collateral), 0, valgrind(1, 5));
  terminate->add(BOOST_TEST_CASE(test_terminate_twice), 0, valgrind(1, 5));
  // See Thread::_step: uncaught_exception is broken, can't make this work. It's
  // a security anyway ...
  // terminate->add(BOOST_TEST_CASE(test_terminate_swallowed), 0, valgrind(1, 5));
  terminate->add(BOOST_TEST_CASE(test_terminate_not_swallowed_unwinding), 0, valgrind(1, 5));
  terminate->add(BOOST_TEST_CASE(test_terminate_not_swallowed_catch), 0, valgrind(1, 5));

  boost::unit_test::test_suite* ni = BOOST_TEST_SUITE("non_interruptible");
  boost::unit_test::framework::master_test_suite().add(ni);
  {
    auto terminate = non_interruptible::terminate;
    ni->add(BOOST_TEST_CASE(terminate), 0, valgrind(1, 5));
    auto nested_exception = non_interruptible::nested_exception;
    ni->add(BOOST_TEST_CASE(nested_exception), 0, valgrind(1, 5));
  }

  boost::unit_test::test_suite* timeout = BOOST_TEST_SUITE("Timeout");
  boost::unit_test::framework::master_test_suite().add(timeout);
  timeout->add(BOOST_TEST_CASE(test_timeout_do), 0, valgrind(1, 5));
  timeout->add(BOOST_TEST_CASE(test_timeout_dont), 0, valgrind(1, 5));
  timeout->add(BOOST_TEST_CASE(test_timeout_aborted), 0, valgrind(3, 2));
  timeout->add(BOOST_TEST_CASE(test_timeout_threw), 0, valgrind(1, 5));
  timeout->add(BOOST_TEST_CASE(test_timeout_finished), 0, valgrind(1, 5));

  boost::unit_test::test_suite* vthread = BOOST_TEST_SUITE("vthread");
  boost::unit_test::framework::master_test_suite().add(vthread);
  vthread->add(BOOST_TEST_CASE(test_vthread), 0, valgrind(1, 5));

#if !defined INFINIT_ANDROID
  boost::unit_test::test_suite* mt = BOOST_TEST_SUITE("multithreading");
  boost::unit_test::framework::master_test_suite().add(mt);
  // mt->add(BOOST_TEST_CASE(test_multithread), 0, valgrind(1, 5));
  mt->add(BOOST_TEST_CASE(test_multithread_spawn_wake), 0, valgrind(1, 5));
  mt->add(BOOST_TEST_CASE(test_multithread_run), 0, valgrind(1, 5));
  mt->add(BOOST_TEST_CASE(test_multithread_run_exception), 0, valgrind(1, 5));
  mt->add(BOOST_TEST_CASE(test_multithread_deadlock_assert), 0, valgrind(1, 5));
#endif

  boost::unit_test::test_suite* sem = BOOST_TEST_SUITE("Semaphore");
  boost::unit_test::framework::master_test_suite().add(sem);
  sem->add(BOOST_TEST_CASE(test_semaphore_noblock), 0, valgrind(1, 5));
  sem->add(BOOST_TEST_CASE(test_semaphore_block), 0, valgrind(1, 5));
  sem->add(BOOST_TEST_CASE(test_semaphore_multi), 0, valgrind(1, 5));

  {
    boost::unit_test::test_suite* mtx = BOOST_TEST_SUITE("mutex");
    boost::unit_test::framework::master_test_suite().add(mtx);
    auto basics = &mutex::basics;
    mtx->add(BOOST_TEST_CASE(basics), 0, valgrind(1, 5));
  }

  boost::unit_test::test_suite* rwmtx = BOOST_TEST_SUITE("RWMutex");
  boost::unit_test::framework::master_test_suite().add(rwmtx);
  rwmtx->add(BOOST_TEST_CASE(test_rw_mutex_multi_read), 0, valgrind(1, 5));
  rwmtx->add(BOOST_TEST_CASE(test_rw_mutex_multi_write), 0, valgrind(1, 5));
  rwmtx->add(BOOST_TEST_CASE(test_rw_mutex_both), 0, valgrind(1, 5));

  boost::unit_test::test_suite* storage = BOOST_TEST_SUITE("Storage");
  boost::unit_test::framework::master_test_suite().add(storage);
  storage->add(BOOST_TEST_CASE(test_storage), 0, valgrind(1, 5));
#if !defined INFINIT_WINDOWS && !defined INFINIT_ANDROID
  storage->add(BOOST_TEST_CASE(test_storage_multithread), 0, valgrind(3, 4));
#endif

  boost::unit_test::test_suite* thread_exception =
    BOOST_TEST_SUITE("thread-exception");
  boost::unit_test::framework::master_test_suite().add(thread_exception);
  thread_exception->add(BOOST_TEST_CASE(thread_exception_test), 0, valgrind(1, 5));

  boost::unit_test::test_suite* io_service = BOOST_TEST_SUITE("io-service");
  boost::unit_test::framework::master_test_suite().add(io_service);
  io_service->add(BOOST_TEST_CASE(test_io_service_throw), 0, valgrind(1, 5));

  boost::unit_test::test_suite* background = BOOST_TEST_SUITE("background");
  boost::unit_test::framework::master_test_suite().add(background);
  {
    using namespace background;
    background->add(BOOST_TEST_CASE(operation), 0, valgrind(3, 10));
    background->add(BOOST_TEST_CASE(operations), 0, valgrind(3, 10));
    background->add(BOOST_TEST_CASE(exception), 0, valgrind(1, 5));
    background->add(BOOST_TEST_CASE(thread_exception_yield), 0, valgrind(1, 5));
    background->add(BOOST_TEST_CASE(aborted), 0, valgrind(1, 5));
    background->add(BOOST_TEST_CASE(aborted_throw), 0, valgrind(1, 5));
    background->add(BOOST_TEST_CASE(future), 0, valgrind(2, 5));
  }

  boost::unit_test::test_suite* released = BOOST_TEST_SUITE("released");
  boost::unit_test::framework::master_test_suite().add(released);
  released->add(BOOST_TEST_CASE(test_released_signal), 0, valgrind(1, 5));

  {
    boost::unit_test::test_suite* tracked = BOOST_TEST_SUITE("tracked");
    boost::unit_test::framework::master_test_suite().add(tracked);
    using namespace tracked;
    tracked->add(BOOST_TEST_CASE(&reset_before), 0, valgrind(1, 5));
    tracked->add(BOOST_TEST_CASE(&reset_after), 0, valgrind(1, 5));
  }

  {
    boost::unit_test::test_suite* s = BOOST_TEST_SUITE("timeout");
    boost::unit_test::framework::master_test_suite().add(s);
    auto timeout = &timeout_::timeout;
    s->add(BOOST_TEST_CASE(timeout), 0, valgrind(1, 5));
    auto complete = &timeout_::complete;
    s->add(BOOST_TEST_CASE(complete), 0, valgrind(1, 5));
    auto race_condition = &timeout_::race_condition;
    s->add(BOOST_TEST_CASE(race_condition), 0, valgrind(1, 5));
  }

#if !defined(INFINIT_WINDOWS) && !defined(INFINIT_IOS)
  {
    boost::unit_test::test_suite* system_signals =
      BOOST_TEST_SUITE("system_signals");
    boost::unit_test::framework::master_test_suite().add(system_signals);
    auto terminate = system_signals::terminate;
    system_signals->add(BOOST_TEST_CASE(terminate), 0, valgrind(1, 5));
  }
#endif

  {
    boost::unit_test::test_suite* s = BOOST_TEST_SUITE("for-each");
    boost::unit_test::framework::master_test_suite().add(s);
    auto parallel = &for_each::parallel;
    s->add(BOOST_TEST_CASE(parallel));
    auto parallel_break = &for_each::parallel_break;
    s->add(BOOST_TEST_CASE(parallel_break));
  }
}
