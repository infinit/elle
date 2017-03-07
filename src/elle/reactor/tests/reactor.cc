#include <condition_variable>
#include <memory>
#include <mutex>

#include "reactor.hh"

#include <elle/finally.hh>
#include <elle/test.hh>

#include <elle/reactor/BackgroundFuture.hh>
#include <elle/reactor/Barrier.hh>
#include <elle/reactor/Channel.hh>
#include <elle/reactor/MultiLockBarrier.hh>
#include <elle/reactor/OrWaitable.hh>
#include <elle/reactor/Scope.hh>
#include <elle/reactor/TimeoutGuard.hh>
#include <elle/reactor/asio.hh>
#include <elle/reactor/duration.hh>
#include <elle/reactor/exception.hh>
#include <elle/reactor/for-each.hh>
#include <elle/reactor/mutex.hh>
#include <elle/reactor/rw-mutex.hh>
#include <elle/reactor/semaphore.hh>
#include <elle/reactor/signal.hh>
#include <elle/reactor/sleep.hh>
#include <elle/reactor/storage.hh>
#include <elle/reactor/thread.hh>
#include <elle/reactor/timer.hh>


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
  elle::reactor::Scheduler sched;
  int step = 0;
  elle::reactor::Thread t(
    sched,
    "coro",
    [&]
    {
      BOOST_CHECK_EQUAL(step, 0);
      ++step;
      elle::reactor::yield();
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
  elle::reactor::yield();
  BOOST_CHECK(step == 2 || step == 3);
  ++step;
  elle::reactor::yield();
  BOOST_CHECK(step == 4);
  ++step;
  elle::reactor::yield();
  BOOST_CHECK(step == 5);
}

static
void
coro2(int& step)
{
  BOOST_CHECK(step == 0 || step == 1);
  ++step;
  elle::reactor::yield();
  BOOST_CHECK(step == 2 || step == 3);
  ++step;
}

static
void
test_basics_interleave()
{
  elle::reactor::Scheduler sched;

  int step = 0;
  elle::reactor::Thread c1(sched, "1", std::bind(coro1, std::ref(step)));
  elle::reactor::Thread c2(sched, "2", std::bind(coro2, std::ref(step)));
  sched.run();
  BOOST_CHECK_EQUAL(step, 5);
}

static
void
nested_schedulers()
{
  elle::reactor::Scheduler outer;

  elle::reactor::Thread t(
    outer, "outer",
    [&]
    {
      BOOST_CHECK(elle::reactor::Scheduler::scheduler() == &outer);
      elle::reactor::Scheduler inner;
      elle::reactor::Thread t(
        inner, "inner",
        [&]
        {
          BOOST_CHECK(elle::reactor::Scheduler::scheduler() == &inner);
        });
      inner.run();
      BOOST_CHECK(elle::reactor::Scheduler::scheduler() == &outer);
    });
  BOOST_CHECK(elle::reactor::Scheduler::scheduler() == nullptr);
  outer.run();
  BOOST_CHECK(elle::reactor::Scheduler::scheduler() == nullptr);
}

ELLE_TEST_SCHEDULED(managed)
{
  elle::reactor::Thread t(
    "thrower",
    [&]
    {
      throw BeaconException();
    },
    elle::reactor::managed = true);
  BOOST_CHECK_THROW(elle::reactor::wait(t), BeaconException);
  BOOST_CHECK_THROW(elle::reactor::wait(t), BeaconException);
}

ELLE_TEST_SCHEDULED_THROWS(non_managed, BeaconException)
{
  elle::reactor::Thread thrower(
    "thrower",
    [&]
    {
      throw BeaconException();
    });
  try
  {
    elle::reactor::sleep();
  }
  catch (...)
  {
    BOOST_CHECK_NO_THROW(elle::reactor::wait(thrower));
    throw;
  }
}

ELLE_TEST_SCHEDULED(unique_ptr)
{
  ELLE_LOG("test null managed pointer")
  {
    elle::reactor::Thread::unique_ptr t;
  }
  ELLE_LOG("terminate thread upon release")
  {
    bool beacon = false;
    elle::reactor::Barrier sleeping;
    elle::reactor::Thread::unique_ptr t(
      new elle::reactor::Thread(
        "waiter",
        [&]
        {
          try
          {
            sleeping.open();
            elle::reactor::sleep();
            BOOST_FAIL("NOT POSSIBLE");
          }
          catch (...)
          {
            beacon = true;
            throw;
          }
        }));
    elle::reactor::wait(sleeping);
    t.reset();
    BOOST_CHECK(beacon);
  }
  ELLE_LOG("terminate disposed thread upon release")
  {
    bool beacon = false;
    elle::reactor::Barrier sleeping;
    elle::reactor::Thread::unique_ptr t(
      new elle::reactor::Thread(
        "waiter",
        [&]
        {
          try
          {
            sleeping.open();
            elle::reactor::sleep();
            BOOST_FAIL("NOT POSSIBLE");
          }
          catch (...)
          {
            beacon = true;
            throw;
          }
        },
        elle::reactor::dispose = true));
    elle::reactor::wait(sleeping);
    t.reset();
    BOOST_CHECK(beacon);
  }
  ELLE_LOG("release disposed thread")
  {
    elle::reactor::Barrier run;
    elle::reactor::Thread::unique_ptr t(
      new elle::reactor::Thread(
        "waiter",
        [&]
        {
          run.open();
        },
        elle::reactor::dispose = true));
    elle::reactor::wait(run);
    BOOST_CHECK(!t);
    t.reset();
  }
  ELLE_LOG("destroy pointer to alive disposed thread")
  {
    elle::reactor::Barrier go;
    auto raw = new elle::reactor::Thread(
      "waiter",
      [&]
      {
        elle::reactor::wait(go);
      },
      elle::reactor::dispose = true);
    auto t = new elle::reactor::Thread::unique_ptr(raw);
    t->release();
    delete t;
    go.open();
    elle::reactor::wait(*raw);
  }
}

ELLE_TEST_SCHEDULED(deadlock)
{
  elle::reactor::Thread::unique_ptr starting;
  elle::reactor::Thread killer(
    "killer",
    [&]
    {
      starting->terminate();
    });
  elle::reactor::yield();
  starting.reset(new elle::reactor::Thread("starting", [] { elle::reactor::sleep(); }));
  elle::reactor::wait(*starting);
}

/*-----.
| Wait |
`-----*/

namespace waitable
{
  class ExceptionNoWait:
    public elle::reactor::Waitable
  {
  public:
    ExceptionNoWait()
    {
      this->_raise<BeaconException>();
    }

    bool
    _wait(elle::reactor::Thread*, Waker const&) override
    {
      return false;
    }
  };

  ELLE_TEST_SCHEDULED(exception_no_wait)
  {
    ExceptionNoWait waitable;
    BOOST_CHECK_THROW(elle::reactor::wait(waitable), BeaconException);
  }

  ELLE_TEST_SCHEDULED(logical_or)
  {
    elle::reactor::Barrier a("A");
    a.open();
    elle::reactor::Barrier b("B");
    b.open();
    ELLE_LOG("both open")
    {
      auto w = a || b;
      elle::reactor::wait(w);
    }
    ELLE_LOG("left closed")
    {
      a.close();
      auto w = a || b;
      elle::reactor::wait(w);
    }
    ELLE_LOG("both closed")
    {
      b.close();
      bool beacon = false;
      auto w = a || b;
      elle::reactor::run_later("open", [&] { BOOST_CHECK(!beacon); a.open(); });
      elle::reactor::wait(w);
      beacon = true;
    }
    ELLE_LOG("right open")
    {
      auto w = a || b;
      elle::reactor::wait(w);
    }
  }

  ELLE_TEST_SCHEDULED(boost_signal)
  {
    boost::signals2::signal<void ()> signal;
    bool beacon = false;
    elle::reactor::Thread waiter(
      "waiter",
      [&]
      {
        elle::reactor::wait(signal);
        beacon = true;
      });
    elle::reactor::yield();
    elle::reactor::yield();
    BOOST_CHECK(!beacon);
    signal();
    BOOST_CHECK(!beacon);
    elle::reactor::yield();
    elle::reactor::yield();
    BOOST_CHECK(beacon);
  }

  ELLE_TEST_SCHEDULED(boost_signal_args)
  {
    auto signal = boost::signals2::signal<void (int i, int j)>{};
    bool beacon = false;
    elle::reactor::Thread waiter(
      "waiter",
      [&]
      {
        elle::reactor::wait(signal, 1, 2);
        beacon = true;
      });
    elle::reactor::yield();
    BOOST_CHECK(!beacon);
    signal(1, 1);
    elle::reactor::yield();
    elle::reactor::yield();
    BOOST_CHECK(!beacon);
    signal(2, 2);
    elle::reactor::yield();
    elle::reactor::yield();
    BOOST_CHECK(!beacon);
    signal(1, 2);
    elle::reactor::yield();
    elle::reactor::yield();
    BOOST_CHECK(beacon);
  }

  ELLE_TEST_SCHEDULED(boost_signal_predicate)
  {
    auto signal = boost::signals2::signal<void (int i, int j)>{};
    bool beacon = false;
    elle::reactor::Thread waiter(
      "waiter",
      [&]
      {
        elle::reactor::wait(signal, [] (int i, int j) { return i + j == 0; });
        beacon = true;
      });
    elle::reactor::yield();
    elle::reactor::yield();
    BOOST_CHECK(!beacon);
    signal(1, 1);
    elle::reactor::yield();
    elle::reactor::yield();
    BOOST_CHECK(!beacon);
    signal(2, -2);
    elle::reactor::yield();
    elle::reactor::yield();
    BOOST_CHECK(beacon);
  }

  ELLE_TEST_SCHEDULED(boost_signal_waiter)
  {
    boost::signals2::signal<void ()> signal;
    auto waiter = elle::reactor::waiter(signal);
    signal();
    elle::reactor::wait(waiter);
  }
}

/*--------.
| Signals |
`--------*/

static
void
waiter(int& step,
       elle::reactor::Waitables& waitables)
{
  BOOST_CHECK_EQUAL(step, 0);
  elle::reactor::wait(waitables);
  ++step;
}

static
void
sender_one(int& step,
           elle::reactor::Signal& s, int expect)
{
  BOOST_CHECK_EQUAL(step, 0);
  elle::reactor::yield();
  BOOST_CHECK_EQUAL(step, 0);
  elle::reactor::yield();
  BOOST_CHECK_EQUAL(step, 0);
  s.signal();
  BOOST_CHECK_EQUAL(step, 0);
  elle::reactor::yield();
  elle::reactor::yield();
  BOOST_CHECK_EQUAL(step, expect);
}

static
void
sender_two(int& step,
           elle::reactor::Signal& s1,
           elle::reactor::Signal& s2)
{
  BOOST_CHECK_EQUAL(step, 0);
  elle::reactor::yield();
  BOOST_CHECK_EQUAL(step, 0);
  elle::reactor::yield();
  BOOST_CHECK_EQUAL(step, 0);
  s1.signal();
  BOOST_CHECK_EQUAL(step, 0);
  elle::reactor::yield();
  BOOST_CHECK_EQUAL(step, 0);
  elle::reactor::yield();
  BOOST_CHECK_EQUAL(step, 0);
  elle::reactor::yield();
  s2.signal();
  elle::reactor::yield();
  elle::reactor::yield();
  BOOST_CHECK_EQUAL(step, 1);
}

static
void
test_signals_one_on_one()
{
  elle::reactor::Scheduler sched;

  int step = 0;
  elle::reactor::Signal signal;
  elle::reactor::Waitables signals;
  signals << signal;
  elle::reactor::Thread w(sched, "waiter",
                    std::bind(waiter, std::ref(step), signals));
  elle::reactor::Thread s(sched, "sender",
                    std::bind(sender_one, std::ref(step),
                              std::ref(signal), 1));
  sched.run();
  BOOST_CHECK_EQUAL(step, 1);
}

static
void
test_signals_one_on_two()
{
  elle::reactor::Scheduler sched;

  int step = 0;
  elle::reactor::Signal signal1;
  elle::reactor::Signal signal2;
  elle::reactor::Waitables signals;
  signals << signal1 << signal2;
  elle::reactor::Thread w(sched, "waiter",
                    std::bind(waiter, std::ref(step), signals));
  elle::reactor::Thread s(sched, "sender",
                    std::bind(sender_two, std::ref(step),
                              std::ref(signal1), std::ref(signal2)));
  sched.run();
  BOOST_CHECK_EQUAL(step, 1);
}

static
void
test_signals_two_on_one()
{
  elle::reactor::Scheduler sched;

  int step = 0;
  elle::reactor::Signal signal;
  elle::reactor::Waitables signals;
  signals << signal;
  elle::reactor::Thread w1(sched, "waiter1",
                     std::bind(waiter, std::ref(step), signals));
  elle::reactor::Thread w2(sched, "waiter2",
                     std::bind(waiter, std::ref(step), signals));
  elle::reactor::Thread s(sched, "sender",
                    std::bind(sender_one, std::ref(step),
                              std::ref(signal), 2));
  sched.run();
  BOOST_CHECK_EQUAL(step, 2);
}

ELLE_TEST_SCHEDULED(test_signals_order)
{
  // check that waiters are signaled in order.
  // try very hard to not pass by cheer luck
  int seq = -1;
  elle::reactor::Signal s;
  elle::reactor::Barrier b;
  auto thread = [&](int id) { b.open(); s.wait(); seq = id;};
  elle::reactor::Thread t1("t1", std::bind(thread, 1));
  b.wait();b.close();
  elle::reactor::Thread t2("t2", std::bind(thread, 2));
  b.wait();b.close();
  elle::reactor::Thread t3("t3", std::bind(thread, 3));
  b.wait();b.close();
  for (unsigned int i = 4; i < 8; ++i)
  {
    new elle::reactor::Thread(elle::sprintf("t%s", i), std::bind(thread, i), true);
    b.wait();b.close();
  }
  elle::reactor::Thread t8("t8", std::bind(thread, 8));
  b.wait();b.close();
  elle::reactor::Thread t9("t9", std::bind(thread, 9));
  b.wait();b.close();
  for (unsigned i=1; i<10; ++i)
  {
    BOOST_CHECK_EQUAL(seq, -1);
    s.signal_one();elle::reactor::yield();elle::reactor::yield();
    BOOST_CHECK_EQUAL(seq, i);
    seq = -1;
    elle::reactor::yield();elle::reactor::yield();
  }
  for (unsigned i=0; i<10; ++i)
    elle::reactor::yield();
}

static
void
waiter_timeout()
{
  elle::reactor::Signal s;
  bool finished = elle::reactor::wait(s, boost::posix_time::milliseconds(10));
  BOOST_CHECK(!finished);
  s.signal();
}

static
void
test_signals_timeout()
{
  elle::reactor::Scheduler sched;

  elle::reactor::Thread t(sched, "waiter", waiter_timeout);
  sched.run();
}

/*--------.
| Barrier |
`--------*/

namespace barrier
{
  ELLE_TEST_SCHEDULED(closed)
  {
    elle::reactor::Barrier barrier;
    bool beacon = false;
    elle::reactor::Thread waiter("waiter", [&] {
        BOOST_CHECK(!barrier.opened());
        elle::reactor::wait(barrier);
        BOOST_CHECK(barrier.opened());
        BOOST_CHECK(!beacon);
        beacon = true;
      });
    elle::reactor::Thread opener("opener", [&] {
        elle::reactor::yield();
        elle::reactor::yield();
        elle::reactor::yield();
        BOOST_CHECK(!beacon);
        barrier.open();
      });
    elle::reactor::wait({waiter, opener});
    BOOST_CHECK(beacon);
  }

  ELLE_TEST_SCHEDULED(opened)
  {
    elle::reactor::Barrier barrier;
    barrier.open();
    BOOST_CHECK(barrier.opened());
    elle::reactor::Thread waiter("waiter", [&] {
        elle::reactor::wait(barrier);
      });
    elle::reactor::wait(waiter);
  }

  ELLE_TEST_SCHEDULED(inverted)
  {
    elle::reactor::Barrier b;
    elle::reactor::wait(!b);
    b.open();
    elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& scope)
    {
      elle::reactor::Barrier timedout;
      scope.run_background(
        "closer",
        [&]
        {
          elle::reactor::wait(timedout);
          b.close();
        });
      BOOST_CHECK(!elle::reactor::wait(!b, 10_ms));
      timedout.open();
      elle::reactor::wait(!b);
    };
  }

  ELLE_TEST_SCHEDULED(exception)
  {
    elle::reactor::Barrier b;
    elle::reactor::Thread waiter(
      "waiter",
      [&] {
        BOOST_CHECK(!b);
        BOOST_CHECK_THROW(elle::reactor::wait(b), BeaconException);
        BOOST_CHECK(b);
        BOOST_CHECK_THROW(elle::reactor::wait(b), BeaconException);
        BOOST_CHECK(b);
      });
    elle::reactor::yield();
    elle::reactor::yield();
    b.raise<BeaconException>();
    elle::reactor::yield();
    elle::reactor::yield();
    BOOST_CHECK_THROW(elle::reactor::wait(b), BeaconException);
    b.open();
    elle::reactor::wait(b);
    b.close();
    elle::reactor::Thread rewaiter(
      "rewaiter",
      [&] {
        BOOST_CHECK(!b);
        elle::reactor::wait(b);
      });
    elle::reactor::yield();
    elle::reactor::yield();
    b.open();
    elle::reactor::wait(rewaiter);
  }

  ELLE_TEST_SCHEDULED(changed)
  {
    elle::reactor::Barrier b;
    bool opened = false;
    b.changed().connect(
      [&] (bool o)
      {
        BOOST_CHECK_EQUAL(opened, !o);
        opened = o;
      });
    b.close();
    b.open();
    BOOST_CHECK(opened);
    b.open();
    b.close();
    BOOST_CHECK(!opened);
    b.close();
  }
}

/*------------------.
| Multilock Barrier |
`------------------*/
static
void
multilock_barrier_basic()
{
  elle::reactor::Scheduler sched;
  elle::reactor::MultiLockBarrier barrier;
  bool no_lock = false;
  bool first_lock = false;
  bool second_lock = false;
  bool third_lock = false;
  bool beacon_waiter = false;
  bool beacon_closer = false;
  elle::reactor::Thread waiter(sched, "waiter", [&] {
      BOOST_CHECK(barrier.opened());
      elle::reactor::wait(barrier);
      no_lock = true;
      elle::reactor::yield();
      elle::reactor::yield();
      BOOST_CHECK_EQUAL(barrier.locks(), 1);
      BOOST_CHECK(first_lock);
      BOOST_CHECK(!second_lock);
      elle::reactor::wait(barrier);
      BOOST_CHECK(barrier.opened());
      BOOST_CHECK(first_lock);
      BOOST_CHECK(second_lock);
      BOOST_CHECK(third_lock);
      beacon_waiter = true;
    });
  elle::reactor::Thread closer(sched, "closer", [&] {
      elle::reactor::yield();
      BOOST_CHECK(no_lock);
      {
        first_lock = true;
        auto lock = barrier.lock();
        elle::reactor::yield();
        {
          second_lock = true;
          auto lock = barrier.lock();
          elle::reactor::yield();
          {
            third_lock = true;
            auto copied_lock = lock;
            elle::reactor::yield();
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
    elle::reactor::Scheduler sched;
    elle::reactor::Thread t(
      sched, "main",
      []
      {
        elle::With<elle::reactor::Scope> s;
      });
    sched.run();
  }

  static
  void
  wait()
  {
    elle::reactor::Scheduler sched;
    elle::reactor::Thread t(
      sched, "main",
      []
      {
        bool beacon1 = false;
        bool beacon2 = false;
        elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& s)
        {
          s.run_background(
            "1",
            [&]
            {
              elle::reactor::yield();
              elle::reactor::yield();
              beacon1 = true;
            });
          elle::reactor::yield();
          s.run_background(
            "2",
            [&]
            {
              elle::reactor::yield();
              elle::reactor::yield();
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
    elle::reactor::Scheduler sched;
    elle::reactor::Thread t(
      sched, "main",
      [wait]
      {
        bool beacon = false;
        elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& s)
        {
          s.run_background(
            "1",
            [&]
            {
              {
                try
                {
                  elle::reactor::yield();
                  elle::reactor::yield();
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
            BOOST_CHECK_THROW(elle::reactor::sleep(), BeaconException);
          BOOST_CHECK(beacon);
        };
      });
    sched.run();
  }

  // Check we get past exceptions even if the scope is done.
  ELLE_TEST_SCHEDULED(exception_done)
  {
    elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& s)
    {
      auto& thread = s.run_background(
        "throw",
        [&]
        {
          throw BeaconException();
        });
      try
      {
        elle::reactor::yield();
        elle::reactor::yield();
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
    elle::reactor::Scheduler sched;
    elle::reactor::Thread t(
      sched, "main",
      []
      {
        bool beacon = false;
        elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& s)
        {
          auto thrower = [] { throw BeaconException(); };
          s.run_background(
            "0",
            [&]
            {
              {
                try
                {
                  elle::reactor::yield();
                  elle::reactor::yield();
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
    elle::reactor::Scheduler sched;
    elle::reactor::Barrier ready;
    elle::reactor::Thread t(
      sched, "main",
      [&]
      {
        elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& s)
        {
          s.run_background(
            "1",
            [&]
            {
              {
                try
                {
                  ready.open();
                  elle::reactor::sleep(1_sec);
                  BOOST_FAIL("should have been killed");
                }
                catch (...)
                {
                  t.terminate();
                  elle::reactor::sleep(1_sec);
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
    elle::reactor::Scheduler sched;
    elle::reactor::Barrier ready;
    elle::reactor::Thread t(
      sched, "main",
      [&]
      {
        elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& s)
        {
          s.run_background(
            "1",
            [&]
            {
              while (true)
                elle::reactor::yield();
            });
          s.run_background(
            "2",
            [&]
            {
              while (true)
                elle::reactor::yield();
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
    elle::reactor::Barrier leave;
    elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& scope)
    {
      scope.run_background(
        "1",
        [&]
        {
          elle::reactor::sleep();
        });
      scope.run_background(
        "2",
        [&]
        {
          throw BeaconException();
        });
      elle::reactor::yield();
      elle::reactor::yield();
      BOOST_CHECK_THROW(
        scope.run_background(
          "3",
          [&]
          {
            elle::reactor::sleep();
          }),
        BeaconException);
      BOOST_CHECK_THROW(elle::reactor::wait(scope), BeaconException);
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
  elle::reactor::Scheduler sched;
  int step = 0;
  elle::reactor::Thread s1(
    sched, "sleeper1",
    [&]
    {
      BOOST_CHECK(step == 0 || step == 1);
      ++step;
      elle::reactor::sleep(valgrind(200_ms, 5));
      BOOST_CHECK_EQUAL(step, 3);
      ++step;
    });
  elle::reactor::Thread s2(
    sched, "sleeper2",
    [&]
    {
      BOOST_CHECK(step == 0 || step == 1);
      ++step;
      elle::reactor::sleep(valgrind(100_ms, 5));
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
  elle::reactor::Duration const delay = valgrind(500_ms, 10);
  // The first sleep is erratic on valgrind, don't include it in the tests.
  if (RUNNING_ON_VALGRIND)
    elle::reactor::sleep(delay);
  for (int i = 0; i < 4; ++i)
  {
    boost::posix_time::ptime start(now());
    elle::reactor::sleep(delay);
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
  elle::reactor::Duration const delay = valgrind(200_ms, 10);
  static const int iter = 5;
  // The first sleep is erratic on valgrind, don't include it in the tests.
  if (RUNNING_ON_VALGRIND)
    elle::reactor::sleep(delay);
  boost::posix_time::ptime start(now());
  int i = 0;
  elle::reactor::Thread::unique_ptr thread = elle::reactor::every(
    delay, "inc",
    [&]
    {
      if (++i >= iter)
        thread->terminate();
    });
  elle::reactor::wait(*thread);
  BOOST_CHECK_EQUAL(i, iter);
  double elapsed = (now() - start).total_milliseconds();
  double expected =  delay.total_milliseconds() * iter;
  BOOST_CHECK_GE(elapsed, expected);
  elle::reactor::sleep(delay * 3);
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
  elle::reactor::yield();
  ++count;
  elle::reactor::yield();
  elle::reactor::yield();
  ++count;
}

static
void
join_waiter(elle::reactor::Thread& thread,
            int& count)
{
  elle::reactor::wait(thread);
  BOOST_CHECK_EQUAL(count, 2);
  ++count;
}

static
void
test_join()
{
  elle::reactor::Scheduler sched;

  int count = 0;
  elle::reactor::Thread j(sched, "joined",
                    std::bind(joined, std::ref(count)));
  elle::reactor::Thread w(sched, "waiter",
                    std::bind(join_waiter,
                              std::ref(j), std::ref(count)));
  sched.run();
  BOOST_CHECK_EQUAL(count, 3);
}

static
void
join_waiter_multiple(elle::reactor::Thread& thread,
                     int& count)
{
  elle::reactor::yield();
  BOOST_CHECK(thread.state() == elle::reactor::Thread::state::done);
  elle::reactor::wait(thread);
  elle::reactor::wait(thread);
  ++count;
}

static
void
test_join_multiple()
{
  elle::reactor::Scheduler sched;

  int count = 0;
  elle::reactor::Thread e(sched, "empty", [] {});
  elle::reactor::Thread w(sched, "waiter",
                    std::bind(join_waiter_multiple,
                              std::ref(e), std::ref(count)));
  sched.run();
  BOOST_CHECK_EQUAL(count, 1);
}

static
void
test_join_timeout()
{
  elle::reactor::Scheduler sched;

  elle::reactor::Thread sb(
    sched, "sleeping beauty",
    [&]
    {
      elle::reactor::sleep(valgrind(200_ms, 10));
    });
  elle::reactor::Thread pc(
    sched, "prince charming",
    [&]
    {
      bool finished = elle::reactor::wait(sb, valgrind(100_ms, 10));
      BOOST_CHECK(!finished);
      BOOST_CHECK(!sb.done());
      finished = elle::reactor::wait(sb, valgrind(200_ms, 10));
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
timeout(elle::reactor::Signal& s,
        bool expect)
{
  bool finished = elle::reactor::wait(s, boost::posix_time::milliseconds(500));
  BOOST_CHECK(finished == expect);
  BOOST_CHECK(s.waiters().empty());
}

static
void
timeout_send(elle::reactor::Signal& s)
{
  elle::reactor::yield();
  BOOST_CHECK_EQUAL(s.waiters().size(), 1);
  s.signal();
}

static
void
test_timeout_do()
{
  elle::reactor::Scheduler sched;

  elle::reactor::Signal s;
  elle::reactor::Thread t(sched, "timeout",
                    std::bind(timeout, std::ref(s), false));
  sched.run();
}

static
void
test_timeout_dont()
{
  elle::reactor::Scheduler sched;

  elle::reactor::Signal s;
  elle::reactor::Thread t(sched, "timeout",
                    std::bind(timeout, std::ref(s), true));
  elle::reactor::Thread p(sched, "poker",
                    std::bind(timeout_send, std::ref(s)));
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
  elle::reactor::Semaphore s(0);
  elle::reactor::Scheduler::scheduler()->current()->wait(
    s, boost::posix_time::milliseconds(1));
}

static
void
schwarzy()
{
  elle::reactor::Scheduler::scheduler()->terminate();
  ::usleep(10);
}

static
void
test_timeout_aborted()
{
  elle::reactor::Scheduler sched;

  boost::asio::deadline_timer(sched.io_service());
  elle::reactor::Thread t1(sched, "John", &connor);
  elle::reactor::Thread t2(sched, "Terminator", &schwarzy);
  sched.run();
}

/*--------------.
| Timeout threw |
`--------------*/

static
void
test_timeout_threw()
{
  elle::reactor::Scheduler sched;
  elle::reactor::Semaphore sem(0);

  elle::reactor::Thread thrower(sched, "thrower", [&] {
      while (!sem.acquire())
        elle::reactor::wait(sem);
      throw BeaconException();
    });
  elle::reactor::Thread waiter(sched, "waiter", [&] {
      sem.release();
      elle::reactor::wait(thrower, 100_ms);
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
  elle::reactor::Scheduler sched;

  elle::reactor::Thread waiter(sched, "waiter", [&] {
      elle::reactor::Sleep s(sched, 10_ms);
      s.start();
      // Block the IO service to make sure the task times out in the same cycle
      // it finishes.
      sched.io_service().post([] { ::usleep(200000); });
      elle::reactor::wait(s, 11_ms);
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
  elle::reactor::Scheduler sched;

  elle::reactor::VThread<int> t(sched, "return value", answer);
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
  elle::reactor::Scheduler sched;
  elle::reactor::Barrier barrier;
  elle::reactor::Thread keeper(sched, "keeper", [&] { elle::reactor::wait(barrier); });
  std::thread s(
    [&]
    {
      ELLE_LOG("wait for the scheduler to be frozen")
        while (keeper.state() != elle::reactor::Thread::state::frozen)
          ::usleep(10000);
      new elle::reactor::Thread(sched, "waker", [&] { barrier.open(); }, true);
    });
  sched.run();
  s.join();
}

static
int
spawned(elle::reactor::Signal& s)
{
  s.signal();
  return 42;
}

static
void
spawn(elle::reactor::Signal& s,
      int& res, elle::reactor::Scheduler& sched)
{
  res = sched.mt_run<int>("spawned", std::bind(spawned, std::ref(s)));
}

static
void
spawner()
{
  elle::reactor::Signal s;
  elle::reactor::Scheduler& sched = *elle::reactor::Scheduler::scheduler();
  int res = 0;
  boost::thread spawner(std::bind(spawn,
                                  std::ref(s), std::ref(res), std::ref(sched)));
  elle::reactor::wait(s);
  spawner.join();
  BOOST_CHECK_EQUAL(res, 42);
}

static
void
test_multithread_run()
{
  elle::reactor::Scheduler sched;

  elle::reactor::Thread t(sched, "spawner", spawner);
  sched.run();
}

static
void
test_multithread_run_exception()
{
  elle::reactor::Scheduler sched;

  elle::reactor::Signal terminate;
  elle::reactor::Thread holder(sched, "spawner",
                         [&] () { sched.current()->wait(terminate); });
  boost::thread runner(std::bind(&elle::reactor::Scheduler::run, &sched));
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
  elle::reactor::Scheduler sched;

  elle::reactor::Thread t(sched, "thread",
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
semaphore_noblock_wait(elle::reactor::Semaphore& s)
{
  BOOST_CHECK_EQUAL(s.count(), 2);
  elle::reactor::wait(s);
  BOOST_CHECK(s.acquire());
  BOOST_CHECK_EQUAL(s.count(), 1);
  elle::reactor::wait(s);
  BOOST_CHECK(s.acquire());
  BOOST_CHECK_EQUAL(s.count(), 0);
}

static
void
test_semaphore_noblock()
{
  elle::reactor::Scheduler sched;
  elle::reactor::Semaphore s(2);
  elle::reactor::Thread wait(sched, "wait",
                       std::bind(&semaphore_noblock_wait, std::ref(s)));
  sched.run();
}

static
void
semaphore_block_wait(elle::reactor::Semaphore& s)
{
  BOOST_CHECK_EQUAL(s.count(), 0);
  while (!s.acquire())
  {
    ELLE_LOG("nope");
    elle::reactor::wait(s);
  }
  ELLE_LOG("yes");
  BOOST_CHECK_EQUAL(s.count(), 0);
}

static
void
semaphore_block_post(elle::reactor::Semaphore& s)
{
  elle::reactor::yield();
  elle::reactor::yield();
  elle::reactor::yield();
  BOOST_CHECK_EQUAL(s.count(), 0);
  ELLE_LOG("release");
  s.release();
  BOOST_CHECK_EQUAL(s.count(), 1);
}

static
void
test_semaphore_block()
{
  elle::reactor::Scheduler sched;
  elle::reactor::Semaphore s;
  elle::reactor::Thread wait(sched, "wait",
                       std::bind(&semaphore_block_wait, std::ref(s)));
  elle::reactor::Thread post(sched, "post",
                       std::bind(&semaphore_block_post, std::ref(s)));
  sched.run();
}

static
void
test_semaphore_multi()
{
  elle::reactor::Scheduler sched;
  elle::reactor::Semaphore s;
  int step = 0;

  auto multi_wait = [&] { while (!s.acquire()) elle::reactor::wait(s); ++step; };

  elle::reactor::Thread wait1(sched, "wait1", multi_wait);
  elle::reactor::Thread wait2(sched, "wait2", multi_wait);
  elle::reactor::Thread post(sched, "post", [&] {
      elle::reactor::yield();
      elle::reactor::yield();
      elle::reactor::yield();
      BOOST_CHECK_EQUAL(s.count(), 0);
      BOOST_CHECK_EQUAL(step, 0);
      s.release();
      BOOST_CHECK_EQUAL(s.count(), 1);
      elle::reactor::yield();
      elle::reactor::yield();
      BOOST_CHECK_EQUAL(s.count(), 0);
      BOOST_CHECK_EQUAL(step, 1);
      s.release();
      elle::reactor::yield();
      elle::reactor::yield();
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
              elle::reactor::Mutex& mutex,
              int yields)
  {
    int count = 0;
    int prev = -1;
    while (count < mutex_yields)
    {
      {
        elle::reactor::Lock lock(mutex);
        // For now, mutex do guarantee fairness between lockers.
        //BOOST_CHECK_NE(i, prev);
        (void)prev;
        BOOST_CHECK_EQUAL(i % 2, 0);
        ++i;
        for (int c = 0; c < yields; ++c)
        {
          ++count;
          elle::reactor::yield();
        }
        ++i;
        prev = i;
      }
      elle::reactor::yield();
    }
  }

  ELLE_TEST_SCHEDULED(basics)
  {
    elle::reactor::Mutex mutex;
    int step = 0;
    elle::reactor::Thread c1("counter1",
                       std::bind(&mutex_count,
                                 std::ref(step), std::ref(mutex), 1));
    elle::reactor::Thread c2("counter2",
                       std::bind(&mutex_count,
                                 std::ref(step), std::ref(mutex), 1));
    elle::reactor::Thread c3("counter3",
                       std::bind(&mutex_count,
                                 std::ref(step), std::ref(mutex), 1));
    elle::reactor::wait({c1, c2, c3});
  }

  ELLE_TEST_SCHEDULED(deadlock)
  {
    elle::reactor::Mutex mutex;
    elle::reactor::Barrier locked;
    elle::reactor::Barrier relocked;
    bool beacon = false;
    elle::reactor::Thread bailer(
      "bailer",
      [&]
      {
        elle::reactor::wait(locked);
        try
        {
          relocked.open();
          elle::reactor::Lock l(mutex);
          BOOST_FAIL("bailer should have been terminated");
        }
        catch (...)
        {
          elle::reactor::Lock l(mutex);
          beacon = true;
          throw;
        }
      });
    elle::reactor::Thread locker(
      "locker",
      [&]
      {
        {
          locked.open();
          elle::reactor::Lock l(mutex);
          elle::reactor::wait(relocked);
        }
        bailer.terminate();
      });
    elle::reactor::wait({bailer, locker});
    BOOST_CHECK(beacon);
  }
}

/*--------.
| RWMutex |
`--------*/

static
void
rw_mutex_read(elle::reactor::RWMutex& mutex,
              int& step)
{
  elle::reactor::Lock lock(mutex);
  ++step;
  elle::reactor::yield();
  BOOST_CHECK_EQUAL(step, 3);
}

static
void
test_rw_mutex_multi_read()
{
  elle::reactor::Scheduler sched;
  elle::reactor::RWMutex mutex;
  int step = 0;
  elle::reactor::Thread r1(sched, "reader1",
                     std::bind(rw_mutex_read,
                               std::ref(mutex), std::ref(step)));
  elle::reactor::Thread r2(sched, "reader2",
                     std::bind(rw_mutex_read,
                               std::ref(mutex), std::ref(step)));
  elle::reactor::Thread r3(sched, "reader3",
                     std::bind(rw_mutex_read,
                               std::ref(mutex), std::ref(step)));
  sched.run();
}

static
void
rw_mutex_write(elle::reactor::RWMutex& mutex,
               int& step)
{
  elle::reactor::Lock lock(mutex.write());
  ++step;
  int prev = step;
  elle::reactor::yield();
  BOOST_CHECK_EQUAL(step, prev);
}

static
void
test_rw_mutex_multi_write()
{
  elle::reactor::Scheduler sched;
  elle::reactor::RWMutex mutex;
  int step = 0;
  elle::reactor::Thread r1(sched, "writer1",
                     std::bind(rw_mutex_write,
                               std::ref(mutex), std::ref(step)));
  elle::reactor::Thread r2(sched, "writer2",
                     std::bind(rw_mutex_write,
                               std::ref(mutex), std::ref(step)));
  elle::reactor::Thread r3(sched, "writer3",
                     std::bind(rw_mutex_write,
                               std::ref(mutex), std::ref(step)));
  sched.run();
}

static
void
rw_mutex_both_read(elle::reactor::RWMutex& mutex,
                   int& step)
{
  elle::reactor::Lock lock(mutex);
  int v = step;
  BOOST_CHECK_EQUAL(v % 2, 0);
  BOOST_CHECK_EQUAL(step, v);
  elle::reactor::yield();
  BOOST_CHECK_EQUAL(step, v);
  elle::reactor::yield();
  BOOST_CHECK_EQUAL(step, v);
}

static
void
rw_mutex_both_write(elle::reactor::RWMutex& mutex,
                    int& step)
{
  elle::reactor::Lock lock(mutex.write());
  ++step;
  elle::reactor::yield();
  elle::reactor::yield();
  ++step;
  BOOST_CHECK_EQUAL(step % 2, 0);
}

static
void
test_rw_mutex_both()
{
  elle::reactor::Scheduler sched;
  elle::reactor::RWMutex mutex;
  int step = 0;
  elle::reactor::Thread r1(sched, "reader1",
                     std::bind(rw_mutex_both_read,
                               std::ref(mutex), std::ref(step)));
  elle::reactor::Thread r2(sched, "reader2",
                     std::bind(rw_mutex_both_read,
                               std::ref(mutex), std::ref(step)));
  sched.step();


  elle::reactor::Thread w1(sched, "writer1",
                     std::bind(rw_mutex_both_write,
                               std::ref(mutex), std::ref(step)));

  elle::reactor::Thread w2(sched, "writer2",
                     std::bind(rw_mutex_both_write,
                               std::ref(mutex), std::ref(step)));
  while (!r1.done())
    sched.step();
  BOOST_CHECK(r2.done());
  sched.step();

  elle::reactor::Thread r3(sched, "reader3",
                     std::bind(rw_mutex_both_read,
                               std::ref(mutex), std::ref(step)));
  elle::reactor::Thread r4(sched, "reader4",
                     std::bind(rw_mutex_both_read,
                               std::ref(mutex), std::ref(step)));
  while (!w1.done() || !w2.done())
    sched.step();

  sched.step();


  elle::reactor::Thread w3(sched, "writer2",
                     std::bind(rw_mutex_both_write,
                               std::ref(mutex), std::ref(step)));

  elle::reactor::Thread w4(sched, "writer4",
                     std::bind(rw_mutex_both_write,
                               std::ref(mutex), std::ref(step)));

  sched.run();
}

/*--------.
| Storage |
`--------*/

static
void
storage(elle::reactor::LocalStorage<int>& val,
        int start)
{
  val.Get() = start;
  elle::reactor::yield();
  BOOST_CHECK_EQUAL(val.Get(), start);
  val.Get()++;
  elle::reactor::yield();
  BOOST_CHECK_EQUAL(val.Get(), start + 1);
}

static
void
test_storage()
{
  elle::reactor::Scheduler sched;
  elle::reactor::LocalStorage<int> val;

  elle::reactor::Thread t1(sched, "1", std::bind(storage, std::ref(val), 0));
  elle::reactor::Thread t2(sched, "2", std::bind(storage, std::ref(val), 1));
  elle::reactor::Thread t3(sched, "3", std::bind(storage, std::ref(val), 2));
  elle::reactor::Thread t4(sched, "4", std::bind(storage, std::ref(val), 3));

  sched.run();
}

// Most likely a wine issue. To be investigated.
#ifndef INFINIT_WINDOWS
static
void
test_storage_multithread()
{
  elle::reactor::LocalStorage<int> val;

  auto coro_action = [&]()
    {
      val.Get() = 0;
    };

  auto action = [&]()
    {
      using namespace boost::posix_time;
      ptime deadline = microsec_clock::local_time() + milliseconds(500);
      elle::reactor::Scheduler sched;
      while (microsec_clock::local_time() < deadline)
      {
        elle::reactor::Thread t(sched, "1", coro_action);
        sched.run();
      }
    };
  std::vector<std::thread> threads;
  try
  {
    for (int i = 0; i < 64; ++i)
      threads.emplace_back(action);
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
  elle::reactor::Scheduler sched1;
  elle::reactor::Scheduler sched2;
  elle::reactor::Scheduler sched3;

  auto action = [&]()
  {
    for (int i = 0; i < 64; ++i)
      elle::reactor::yield();
  };

  elle::reactor::Thread thread1(sched1, "Thread 1", action);
  elle::reactor::Thread thread2(sched2, "Thread 2", action);
  elle::reactor::Thread thread3(sched3, "Thread 3", action);

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
  elle::reactor::Scheduler sched;
  elle::reactor::Thread thread(sched, "Exception Tester", &except_gen);
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
  BOOST_CHECK_EQUAL(thread.state(), elle::reactor::Thread::state::done);
}


static
void
exception_yield_pattern(std::vector<unsigned int> yield_pattern,
                        std::vector<bool> enable_pattern,
                        std::vector<unsigned int> no_exception)
{
  elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& s)
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
                elle::reactor::yield();
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
                elle::reactor::yield();
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
                elle::reactor::yield();
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
                  elle::reactor::yield();
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
          elle::reactor::yield();
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
  elle::reactor::Scheduler sched;
  bool beacon = false;
  elle::reactor::Thread t(
    sched, "thread",
    [&]
    {
      try
      {
        elle::reactor::yield();
      }
      catch (elle::reactor::Terminate const&)
      {
        elle::reactor::yield();
        beacon = true;
        throw;
      }
      BOOST_ERROR("thread wasn't terminated");
    });
  elle::reactor::Thread term(
    sched, "terminate",
    [&]
    {
      elle::reactor::Scheduler& sched = *elle::reactor::Scheduler::scheduler();
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
  elle::reactor::Barrier sleeping;
  bool beacon = false;
  elle::reactor::Thread t(
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
          elle::reactor::sleep();
        }
      }
      catch (...)
      {
        ELLE_LOG("delay termination");
        elle::reactor::sleep(boost::posix_time::milliseconds(10));
        beacon = true;
        ELLE_LOG("actually die");
        throw;
      }
    });
  BOOST_CHECK_EQUAL(t.state(), elle::reactor::Thread::state::running);
  elle::reactor::wait(sleeping);
  t.terminate_now();
  BOOST_CHECK(beacon);
}

/*------------------------.
| Terminate now destroyed |
`------------------------*/

ELLE_TEST_SCHEDULED(test_terminate_now_destroyed)
{
  elle::reactor::Barrier finish;
  elle::reactor::Thread t1(
    "t1",
    [&]
    {
      // Ignore the first Terminate (sent by t2) and catch the second one (by
      // t4).
      try
      {
        ELLE_LOG("sleep")
          elle::reactor::sleep();
      }
      catch (...)
      {
        ELLE_LOG("block until test end")
          finish.wait();
      }
    });
  elle::reactor::Thread t2(
    "t2",
    [&]
    {
      ELLE_LOG("terminate thread 1")
        t1.terminate_now();
      BOOST_ERROR("terminate_now should have failed (killed by t3 during wait)");
    });
  elle::reactor::Thread t3(
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
    if (t3.state() == elle::reactor::Thread::state::frozen)
    {
      finish.open();
      break;
    }
    BOOST_CHECK(!t1.done());
    BOOST_CHECK(!t2.done());
    elle::reactor::yield();
  }
  elle::reactor::wait(t3);
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
      elle::reactor::yield();
    }
  }
  catch (...)
  {
    terminated = true;
    elle::reactor::sleep(boost::posix_time::milliseconds(10));
    throw;
  }
}

static
void
terminate_now(elle::reactor::Thread& t, bool& terminated)
{
  t.terminate_now();
  BOOST_CHECK(terminated);
}

static
void
test_terminate_now_disposed()
{
  elle::reactor::Scheduler sched;
  bool beacon = false;
  auto* t = new elle::reactor::Thread(sched, "terminated",
                                std::bind(&terminated, std::ref(beacon)), true);
  elle::reactor::Thread terminate(sched, "terminate", std::bind(&terminate_now,
                                                          std::ref(*t),
                                                          std::ref(beacon)));


  sched.run();
}

/*-----------------------.
| Terminate now starting |
`-----------------------*/

ELLE_TEST_SCHEDULED(test_terminate_now_starting_dispose)
{
  auto t = new elle::reactor::Thread("starting", [] {}, true);
  t->terminate_now();
}

static
void
test_terminate_now_starting()
{
  bool beacon = true;
  elle::reactor::Scheduler sched;

  elle::reactor::Thread starting(sched, "starting", [] {});
  elle::reactor::Thread terminate(sched, "terminate", std::bind(&terminate_now,
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
  elle::reactor::Scheduler sched;
  std::unique_ptr<elle::reactor::Thread> starting;
  elle::reactor::Thread terminate(sched, "terminate",
                            [&] { starting->terminate_now(); });
  starting.reset(
    new elle::reactor::Thread(sched, "starting",
                        [&] { beacon = false; elle::reactor::yield(); }));
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
killer(elle::reactor::Thread& v)
{
  v.terminate_now();
}

static
void
test_terminate_now_scheduled()
{
  elle::reactor::Scheduler sched;

  // Make sure v is run after k in the first round, to check if terminating it
  // removes it from the running thread for this round.
  elle::reactor::Thread* g = nullptr;
  elle::reactor::Thread k(sched, "Killer", [&]() { killer(*g); });
  elle::reactor::Thread v(sched, "Victim", &victim);
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
  elle::reactor::Scheduler sched;

  elle::reactor::Thread t(sched, "thrower", except_gen);

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
    elle::reactor::yield();
  }
  catch (...)
  {
    ++beacon;
    elle::reactor::yield();
    ++beacon;
  }
}

static
void
test_exception_escape_collateral()
{
  elle::reactor::Scheduler sched;
  int beacon = 0;

  elle::reactor::Thread t1(sched, "collateral",
                     std::bind(exception_escape_collateral, std::ref(beacon)));
  elle::reactor::Thread t2(sched, "thrower", except_gen);

  BOOST_CHECK_THROW(sched.run(), BeaconException);
  BOOST_CHECK_EQUAL(beacon, 3);
}

// Check thread are not re-terminated.
ELLE_TEST_SCHEDULED(test_terminate_twice)
{
  elle::reactor::Barrier waiting;
  elle::reactor::Barrier reterminated;
  elle::reactor::Barrier rewaiting;
  elle::reactor::Thread thread(
    "waiter",
    [&]
    {
      try
      {
        waiting.open();
        elle::reactor::sleep();
      }
      catch (elle::reactor::Terminate const&)
      {
        rewaiting.open();
        BOOST_CHECK_NO_THROW(elle::reactor::wait(reterminated));
        throw;
      }
    });
  elle::reactor::wait(waiting);
  thread.terminate();
  elle::reactor::wait(rewaiting);
  thread.terminate();
  reterminated.open();
  elle::reactor::wait(thread);
}

// Check exception swallowing mechanism false positives.
ELLE_TEST_SCHEDULED(test_terminate_not_swallowed_unwinding)
{
  bool beacon = false;
  elle::reactor::Barrier waiting;
  class Waiter
  {
  public:
    Waiter(bool& beacon)
      : _beacon(beacon)
    {}

    ~Waiter()
    {
      elle::reactor::yield();
      this->_beacon = true;
    }
  private:
    bool& _beacon;
  };
  elle::reactor::Thread thread(
    "survivor",
    [&]
    {
      Waiter waiter(beacon);
      waiting.open();
      elle::reactor::sleep();
    });
  elle::reactor::wait(waiting);
  thread.terminate_now();
  BOOST_CHECK(beacon);
}

// Check exception swallowing mechanism false positives.
ELLE_TEST_SCHEDULED(test_terminate_not_swallowed_catch)
{
  bool beacon = false;
  elle::reactor::Barrier waiting;
  elle::reactor::Thread thread(
    "survivor",
    [&]
    {
      waiting.open();
      try
      {
        elle::reactor::sleep();
      }
      catch (elle::reactor::Terminate const&)
      {
        elle::reactor::yield();
        beacon = true;
      }
    });
  elle::reactor::wait(waiting);
  thread.terminate_now();
  BOOST_CHECK(beacon);
}

// Check you can't swallow terminate exception.
// ELLE_TEST_SCHEDULED(test_terminate_swallowed)
// {
//   int i = 0;
//   int const n = 8;
//   elle::reactor::Barrier waiting;
//   elle::reactor::Thread thread(
//     "survivor",
//     [&]
//     {
//       while (++i < n)
//       {
//         try
//         {
//           waiting.open();
//           elle::reactor::sleep();
//         }
//         catch (elle::reactor::Terminate const&)
//         {}
//       }
//     });
//   elle::reactor::wait(waiting);
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
  auto& sched = *elle::reactor::Scheduler::scheduler();
  sched.io_service().post(&except_gen);
  elle::reactor::yield();
  beacon = true;
}

static
void
test_io_service_throw()
{
  bool beacon = false;
  elle::reactor::Scheduler sched;
  elle::reactor::Thread thread(sched, "Poster", std::bind(&poster, std::ref(beacon)));
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
    elle::reactor::Thread counter(
      "counter",
      [&]
      {
        do
        {
          std::unique_lock<std::mutex> lock(mutex);
          if (backgrounded)
            break;
          elle::reactor::yield();
        }
        while (true);
        ++i;
        elle::reactor::yield();
        ++i;
        elle::reactor::yield();
        ++i;
        BOOST_CHECK(backgrounded);
        std::unique_lock<std::mutex> lock(mutex);
        cv.notify_one();
      });
    elle::reactor::background([&]
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
    elle::reactor::Duration sleep_time = valgrind(500_ms, 5);
    // The first sleep is erratic on valgrind, don't include it in the
    // tests.
    if (RUNNING_ON_VALGRIND)
      elle::reactor::sleep(sleep_time);
    // Run it three times to check the thread pool doesn't exceed 16.
    for (int run = 0; run < 3; ++run)
    {
      int count = 0;
      std::vector<elle::reactor::Thread*> threads;
      for (int i = 0; i < iterations; ++i)
        threads.push_back(
          new elle::reactor::Thread(
            elle::sprintf("thread %s", i),
            [&count, &sleep_time]
            {
              elle::reactor::background(
                [&]
                {
                  ::usleep(sleep_time.total_microseconds());
                });
              ++count;
            }));
      auto start = boost::posix_time::microsec_clock::local_time();
      elle::reactor::wait(elle::reactor::Waitables(begin(threads), end(threads)));
      auto duration =
        boost::posix_time::microsec_clock::local_time() - start;
      BOOST_CHECK_EQUAL(count, iterations);
      BOOST_CHECK_EQUAL(
        elle::reactor::scheduler().background_pool_size(), iterations);
      BOOST_CHECK_LT(duration, sleep_time * 3);
      for (auto thread: threads)
        delete thread;
    }
  }

  ELLE_TEST_SCHEDULED(exception)
  {
    BOOST_CHECK_THROW(elle::reactor::background([] { throw BeaconException(); }),
                      std::exception);
    BOOST_CHECK_THROW(elle::reactor::background([] { throw BeaconException(); }),
                      std::exception);
    BOOST_CHECK_THROW(elle::reactor::background([] { throw BeaconException(); }),
                      std::exception);
    BOOST_CHECK_EQUAL(elle::reactor::scheduler().background_pool_size(), 1);
  }

  ELLE_TEST_SCHEDULED(aborted)
  {
    elle::reactor::Thread main(
      "main",
      [&]
      {
        auto done = std::make_shared<bool>(false);
        auto const sleep_time = 500_ms;
        try
        {
          elle::reactor::background([done, sleep_time]
                              {
                                ::usleep(sleep_time.total_microseconds());
                                *done = true;
                              });
        }
        catch (elle::reactor::Terminate const&)
        {
          BOOST_CHECK(!*done);
          return;
        }
        BOOST_ERROR("background task was not terminated");
      });
    elle::reactor::Thread kill(
      "kill",
      [&]
      {
        main.terminate();
      });
    elle::reactor::wait(main);
  }

  ELLE_TEST_SCHEDULED(aborted_throw)
  {
    elle::reactor::Barrier backgrounded;
    bool terminated = false;
    std::mutex mtx;
    std::condition_variable barrier;
    elle::reactor::Thread background(
      "background",
      [&]
      {
        backgrounded.open();
        elle::reactor::background(
          [&]
          {
            // Wait for the "background" elle::reactor::Thread to be terminated.
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
    elle::reactor::wait(backgrounded);
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
      elle::reactor::BackgroundFuture<int> f(42);
      BOOST_CHECK_EQUAL(f.value(), 42);
    }
    ELLE_LOG("test waiting value")
    {
      elle::reactor::BackgroundFuture<int> f([] { ::usleep(100000); return 51; });
      BOOST_CHECK_EQUAL(f.value(), 51);
      f = [] { ::usleep(100000); return 69; };
      BOOST_CHECK_EQUAL(f.value(), 69);
    }
    ELLE_LOG("test already available value")
    {
      elle::reactor::BackgroundFuture<int> f([] { return 69; });
      elle::reactor::sleep(200_ms);
      BOOST_CHECK_EQUAL(f.value(), 69);
    }
    ELLE_LOG("test killing unfinished")
    {
      elle::reactor::Barrier destroying;
      bool sleeping = false;
      bool over = false;
      std::condition_variable sync;
      std::mutex mtx;
      auto f = std::unique_ptr<elle::reactor::BackgroundFuture<int>>{};
      elle::reactor::Thread t(
        "future",
        [&]
        {
          f = std::make_unique<elle::reactor::BackgroundFuture<int>>(
            [&]
            {
              {
                std::unique_lock<std::mutex> lock(mtx);
                sleeping = true;
                sync.notify_one();
              }
              // Wait for the main thread to have killed t.
              {
                std::unique_lock<std::mutex> lock(mtx);
                sync.wait(lock, [&] { return over; });
              }
              return 42;
            });
          {
            std::unique_lock<std::mutex> lock(mtx);
            sync.wait(lock, [&] { return sleeping; });
          }
          destroying.open();
        });
      elle::reactor::wait(destroying);
      t.terminate_now();
      {
        std::unique_lock<std::mutex> lock(mtx);
        over = true;
        sync.notify_one();
      }
      // Wait for the background task to be done before claiming its
      // memory.
      f->value();
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
    elle::reactor::Scheduler sched;
    elle::reactor::Thread t(sched, "main", [&t] ()
                      {
                        ::kill(::getpid(), SIGINT);
                        elle::reactor::wait(t);
                      });
    sched.signal_handle(SIGINT, [&sched] { sched.terminate(); });
    sched.run();
  }
}
#endif

ELLE_TEST_SCHEDULED(test_simple_channel)
{

  elle::reactor::Channel<int> channel;

  elle::With<elle::reactor::Scope>() << [&](elle::reactor::Scope &s)
  {
    s.run_background("producer", [&]() { channel.put(42); });

    s.run_background("consumer", [&]() {
               int final = channel.get();
               BOOST_CHECK_EQUAL(final, 42);
            });

    elle::reactor::wait(s);
  };
}

ELLE_TEST_SCHEDULED(test_multiple_channel)
{
  elle::reactor::Channel<int> channel;
  elle::reactor::Barrier reading;

  elle::With<elle::reactor::Scope>() << [&](elle::reactor::Scope &s)
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
                       elle::reactor::wait(reading);
                       for (int i: list)
                         channel.put(i);
                     });
   elle::reactor::wait(s);
  };
}


ELLE_TEST_SCHEDULED(test_multiple_consumers)
{
  elle::reactor::Channel<int> channel;
  bool got = false;
  elle::With<elle::reactor::Scope>() << [&](elle::reactor::Scope &s)
  {
    auto action = [&]
      {
        BOOST_CHECK_EQUAL(channel.get(), 42);
        if (!got)
        {
          got = true;
          elle::reactor::yield();
          elle::reactor::yield();
          elle::reactor::Scheduler::scheduler()->terminate();
        }
        else
          BOOST_FAIL("value was read twice");
      };
    s.run_background("consumer 1", action);
    s.run_background("consumer 2", action);
    s.run_background("producer", [&] { channel.put(42); });
    elle::reactor::wait(s);
  };
}

namespace channel
{
  ELLE_TEST_SCHEDULED(wake_clear)
  {
    elle::reactor::Channel<int> channel;
    elle::With<elle::reactor::Scope>() << [&](elle::reactor::Scope &s)
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
          elle::reactor::yield();
          elle::reactor::yield();
          channel.put(51);
        });
    elle::reactor::wait(s);
    };
  }

  ELLE_TEST_SCHEDULED(open_close)
  {
    elle::reactor::Channel<int> channel;
    elle::reactor::Signal gotcha;
    elle::With<elle::reactor::Scope>() << [&](elle::reactor::Scope &s)
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
          elle::reactor::wait(gotcha);
          channel.close();
          channel.put(1);
          BOOST_CHECK(!elle::reactor::wait(gotcha, 500_ms));
          channel.open();
          channel.put(2);
          elle::reactor::wait(gotcha);
        });
    elle::reactor::wait(s);
    };
  }
}

ELLE_TEST_SCHEDULED(test_released_signal)
{
  using elle::reactor::Thread;
  {
    bool beacon = false;
    Thread t("test 1", [&] { elle::reactor::yield(); });
    t.released().connect([&] { beacon = true; });
    BOOST_CHECK(!beacon);
    elle::reactor::wait(t);
    BOOST_CHECK(beacon);
  }
  // terminate_now from thread
  {
    bool beacon = false;
    Thread t("test 2", [&] { t.terminate_now(true); });
    t.released().connect([&] { beacon = true; });
    BOOST_CHECK(!beacon);
    elle::reactor::wait(t);
    BOOST_CHECK(beacon);
  }
  // terminate_now
  {
    bool beacon = false;
    Thread t("test 3", [&] { elle::reactor::sleep(); });
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
    auto t = elle::reactor::Thread::make_tracked("test", [] {});
    t.reset();
    elle::reactor::yield();
    elle::reactor::yield();
    elle::reactor::yield();
  }

  ELLE_TEST_SCHEDULED(reset_after)
  {
    auto t = elle::reactor::Thread::make_tracked("test", [] {});
    elle::reactor::yield();
    elle::reactor::yield();
    elle::reactor::yield();
    t.reset();
  }
}

static
void
coro(int& step)
{
  BOOST_CHECK_EQUAL(step, 0);
  ++step;
  elle::reactor::yield();
  BOOST_CHECK_EQUAL(step, 1);
  ++step;
}

namespace timer
{
  using elle::reactor::Timer;

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
    elle::reactor::Barrier started;
    {
      Timer t("myTimer2", 0_ms,
              [&]
              {
                started.open();
                elle::reactor::yield();
                elle::reactor::yield();
                elle::reactor::yield();
                elle::reactor::yield();
                v = true;
              });
      elle::reactor::wait(started);
    }
    BOOST_CHECK(v);
  }

  ELLE_TEST_SCHEDULED(basic_cancel)
  {
    int v = 0;
    Timer t("myTimer3", 100_ms, std::bind(&coro, std::ref(v)));
    BOOST_CHECK_EQUAL(v, 0);
    t.cancel();
    elle::reactor::sleep(200_ms);
    BOOST_CHECK_EQUAL(v, 0);
  }

  ELLE_TEST_SCHEDULED(cancel_after_start)
  {
    elle::reactor::Barrier b;
    elle::reactor::Barrier b2;
    int v = 0;
    Timer t("myTimer4", 0_ms, [&] { b.open(); v = 1; b2.wait(); v=2;});
    b.wait();
    BOOST_CHECK_EQUAL(v, 1);
    t.cancel();
    b2.open();
    elle::reactor::yield();
    elle::reactor::yield();
    BOOST_CHECK_EQUAL(v, 2);
  }

  ELLE_TEST_SCHEDULED(cancel_now_after_start)
  {
    elle::reactor::Barrier b;
    int v = 0;
    Timer t("myTimer5", 0_ms, [&] { b.open(); v = 1; elle::reactor::yield(); elle::reactor::yield(); v=2;});
    b.wait();
    t.cancel_now(); // Waits.
    BOOST_CHECK_EQUAL(v, 2);
  }

  ELLE_TEST_SCHEDULED(terminate_after_start)
  {
    int v = 0;
    elle::reactor::Barrier b;
    Timer t("myTimer6", 0_ms, [&]
            {
              try {
                b.open(); v = 1; elle::reactor::yield(); elle::reactor::yield(); v=2;
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
    elle::reactor::yield(); elle::reactor::yield();
    BOOST_CHECK_EQUAL(v, 3);
  }

  ELLE_TEST_SCHEDULED(terminate_now_after_start)
  {
    int v = 0;
    elle::reactor::Barrier b;
    Timer t("myTimer7", 0_ms, [&]
            {
              try {
                b.open(); v = 1; elle::reactor::yield(); elle::reactor::yield(); v=2;
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
    elle::With<elle::reactor::Scope>() << [] (elle::reactor::Scope& scope)
    {
      bool beacon1 = false;
      bool beacon2 = false;
      scope.run_background(
        "control",
        [&]
        {
          elle::reactor::sleep(valgrind(50_ms, 20));
          beacon1 = true;
          elle::reactor::sleep(valgrind(100_ms, 20));
          beacon2 = true;
        });
      try
      {
        elle::reactor::TimeoutGuard timeout(valgrind(100_ms, 20));
        elle::reactor::sleep(valgrind(200_ms, 20));
        BOOST_ERROR("didn't timeout");
      }
      catch(elle::reactor::Timeout const&)
      {}
      BOOST_CHECK(beacon1);
      BOOST_CHECK(!beacon2);
    };
  }

  ELLE_TEST_SCHEDULED(complete)
  {
    elle::With<elle::reactor::Scope>() << [] (elle::reactor::Scope& scope)
    {
      try
      {
        elle::reactor::TimeoutGuard timeout(500_ms);
        elle::reactor::sleep(100_ms);
      }
      catch(elle::reactor::Timeout const&)
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
      elle::reactor::TimeoutGuard timeout(sleep_time);
      ::usleep((sleep_time * 2).total_microseconds());
    }
    catch(elle::reactor::Timeout const&)
    {
      BOOST_ERROR("timeout");
    }
  }
}

namespace non_interruptible
{
  ELLE_TEST_SCHEDULED(terminate)
  {
    ELLE_LOG("test 1")
    {
      elle::reactor::Barrier b,c;
      int status = 0;
      elle::reactor::Thread t(
        "test",
        [&]
        {
          elle::With<elle::reactor::Thread::NonInterruptible>() << [&]
          {
            status = 1;
            b.open();
            elle::reactor::wait(c);
            status = 2;
          };
        });
      elle::reactor::wait(b);
      b.close(); c.open();
      t.terminate_now();
      BOOST_CHECK_EQUAL(status, 2);
    }
    ELLE_LOG("test 2")
    {
      elle::reactor::Barrier b,c,d;
      int status = 0;
      elle::reactor::Thread t(
        "test",
        [&]
        {
          elle::With<elle::reactor::Thread::NonInterruptible>() << [&]
          {
            status = 1;
            b.open();
            elle::reactor::wait(c);
            status = 2;
          };
          elle::reactor::wait(d);
          status = 3;
        });
      elle::reactor::wait(b);
      c.open();
      elle::reactor::yield();
      t.terminate_now();
      BOOST_CHECK_EQUAL(status, 2);
    }
    ELLE_LOG("test 3")
    {
      elle::reactor::Scheduler sched;
      elle::reactor::Barrier ready("ready"), go("go"), terminated("terminated");
      elle::reactor::Thread t(
        sched, "main",
        [&]
        {
          elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& s)
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
                    elle::With<elle::reactor::Thread::NonInterruptible>() << [&]
                    {
                      // Because it's not interruptible, reactor should wait for
                      // ever...
                      ELLE_TRACE("wait for %s", go)
                        go.wait();
                      ELLE_TRACE("go status: %s", (bool) go);
                    };
                  }
                }
                catch (elle::reactor::Terminate const&)
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
                elle::reactor::yield();
                // Terminate shoulb be blocked until go is open.
                ELLE_TRACE("terminate %s", t1)
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
                elle::reactor::yield();
                elle::reactor::yield();
                BOOST_CHECK(!t1.done());
                BOOST_CHECK(!t2.done());
                ELLE_TRACE("open %s", go)
                  go.open();
                BOOST_CHECK(!t1.done());
                BOOST_CHECK(!t2.done());
                ELLE_TRACE("wait fo %s", terminated)
                  terminated.wait(1_sec);
                elle::reactor::yield();
                elle::reactor::yield();
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
    elle::reactor::Barrier sleeping;
    elle::reactor::Channel<int> poke;
    elle::reactor::Thread cobaye(
      "cobaye",
      [&]
      {
        try
        {
          elle::With<elle::reactor::Thread::NonInterruptible>() << [&]
          {
            sleeping.open();
            poke.get();
            throw BeaconException();
          };
        }
        catch (BeaconException const&)
        {}
        catch (elle::reactor::Terminate const&)
        {
          beacon = true;
        }
      });
    elle::reactor::wait(sleeping);
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
    elle::reactor::Barrier b;
    elle::reactor::Thread check(
      "check",
      [&]
      {
        BOOST_CHECK_EQUAL(c, std::vector<int>({0, 1, 2}));
        elle::reactor::yield();
        BOOST_CHECK_EQUAL(c, std::vector<int>({1, 2, 3}));
        elle::reactor::yield();
        b.open();
      });
    elle::reactor::for_each_parallel(
      c,
      [&] (int& c)
      {
        ++c;
        elle::reactor::wait(b);
      });
    BOOST_CHECK(b.opened());
  }

  ELLE_TEST_SCHEDULED(parallel_break)
  {
    std::vector<int> c{0, 1, 2};
    elle::reactor::for_each_parallel(
      c,
      [&] (int& c)
      {
        if (c == 1)
          elle::reactor::break_parallel();
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
    boost::unit_test::test_suite* subsuite = BOOST_TEST_SUITE("wait");
    boost::unit_test::framework::master_test_suite().add(subsuite);
    using namespace waitable;
    subsuite->add(BOOST_TEST_CASE(exception_no_wait), 0, valgrind(1, 5));
    subsuite->add(BOOST_TEST_CASE(logical_or), 0, valgrind(1, 5));
    subsuite->add(BOOST_TEST_CASE(boost_signal), 0, valgrind(1, 5));
    subsuite->add(BOOST_TEST_CASE(boost_signal_args), 0, valgrind(1, 5));
    subsuite->add(BOOST_TEST_CASE(boost_signal_predicate), 0, valgrind(1, 5));
    subsuite->add(BOOST_TEST_CASE(boost_signal_waiter), 0, valgrind(1, 5));
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
    barrier->add(BOOST_TEST_CASE(changed), 0, valgrind(1, 5));
    barrier->add(BOOST_TEST_CASE(closed), 0, valgrind(1, 5));
    barrier->add(BOOST_TEST_CASE(exception), 0, valgrind(1, 5));
    barrier->add(BOOST_TEST_CASE(inverted), 0, valgrind(1, 5));
    barrier->add(BOOST_TEST_CASE(opened), 0, valgrind(1, 5));
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
    auto deadlock = &mutex::deadlock;
    mtx->add(BOOST_TEST_CASE(deadlock), 0, valgrind(1, 5));
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
    background->add(BOOST_TEST_CASE(aborted), 0, valgrind(1, 5));
    background->add(BOOST_TEST_CASE(aborted_throw), 0, valgrind(1, 5));
    background->add(BOOST_TEST_CASE(exception), 0, valgrind(1, 5));
    background->add(BOOST_TEST_CASE(future), 0, valgrind(2, 5));
    background->add(BOOST_TEST_CASE(operation), 0, valgrind(3, 10));
    background->add(BOOST_TEST_CASE(operations), 0, valgrind(3, 10));
    background->add(BOOST_TEST_CASE(thread_exception_yield), 0, valgrind(1, 5));
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
