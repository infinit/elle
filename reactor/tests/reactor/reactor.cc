#include "reactor.hh"

#include <reactor/semaphore.hh>
#include <reactor/mutex.hh>
#include <reactor/rw-mutex.hh>
#include <reactor/signal.hh>
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

void
yield()
{
  reactor::Scheduler::scheduler()->current()->yield();
}

bool
wait(reactor::Waitable& s,
     reactor::DurationOpt timeout = reactor::DurationOpt())
{
  return sched->current()->wait(s, timeout);
}

void
wait(reactor::Waitables& s)
{
  sched->current()->wait(s);
}

void
sleep(reactor::Duration d)
{
  sched->current()->sleep(d);
}

/*--------.
| Helpers |
`--------*/

void
empty()
{}

/*-------.
| Basics |
`-------*/

void
coro(int& step)
{
  BOOST_CHECK_EQUAL(step, 0);
  ++step;
  yield();
  BOOST_CHECK_EQUAL(step, 1);
  ++step;
}

void
test_basics_one()
{
  Fixture f;

  int step = 0;
  reactor::Thread t(*sched, "coro", boost::bind(coro, boost::ref(step)));
  sched->run();
  BOOST_CHECK_EQUAL(step, 2);
}

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

void
coro2(int& step)
{
  BOOST_CHECK(step == 0 || step == 1);
  ++step;
  yield();
  BOOST_CHECK(step == 2 || step == 3);
  ++step;
}

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

void
waiter(int& step,
       reactor::Waitables& waitables)
{
  BOOST_CHECK_EQUAL(step, 0);
  sched->current()->wait(waitables);
  ++step;
}

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

void
waiter_timeout()
{
  reactor::Signal s;
  bool finished = wait(s, boost::posix_time::milliseconds(10));
  BOOST_CHECK(!finished);
  s.signal();
}

void
test_signals_timeout()
{
  Fixture f;

  reactor::Thread t(*sched, "waiter", waiter_timeout);
  sched->run();
}

/*------.
| Sleep |
`------*/

void
sleeper1(int& step)
{
  BOOST_CHECK(step == 0 || step == 1);
  ++step;
  sleep(boost::posix_time::milliseconds(400));
  BOOST_CHECK_EQUAL(step, 3);
  ++step;
}

void
sleeper2(int& step)
{
  BOOST_CHECK(step == 0 || step == 1);
  ++step;
  sleep(boost::posix_time::milliseconds(200));
  BOOST_CHECK_EQUAL(step, 2);
  ++step;
}

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

void
sleep_timer(int& iterations)
{
  reactor::Duration delay(boost::posix_time::milliseconds(100));

  while (--iterations)
  {
    boost::posix_time::ptime start(now());
    sleep(delay);
    double elapsed = (now() - start).total_milliseconds();
    double expected =  delay.total_milliseconds();
    BOOST_CHECK_CLOSE(elapsed, expected, double(10));
  }
}

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

void
join_waiter(reactor::Thread& thread,
            int& count)
{
  wait(thread);
  BOOST_CHECK_EQUAL(count, 2);
  ++count;
}

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

void
join_waiter_multiple(reactor::Thread& thread,
                     int& count)
{
  yield();
  BOOST_CHECK(thread.state() == reactor::Thread::state::done);
  wait(thread);
  wait(thread);
  ++count;
}

void
test_join_multiple()
{
  Fixture f;

  int count = 0;
  reactor::Thread e(*sched, "empty", empty);
  reactor::Thread w(*sched, "waiter",
                    boost::bind(join_waiter_multiple,
                                boost::ref(e), boost::ref(count)));
  sched->run();
  BOOST_CHECK_EQUAL(count, 1);
}

void
sleeping_beauty()
{
  sleep(boost::posix_time::milliseconds(100));
}

void
prince_charming(reactor::Thread& sleeping_beauty)
{
  bool finished = wait(sleeping_beauty, boost::posix_time::milliseconds(50));
  BOOST_CHECK(!finished);
  BOOST_CHECK(!sleeping_beauty.done());
  finished = wait(sleeping_beauty, boost::posix_time::milliseconds(200));
  BOOST_CHECK(finished);
  BOOST_CHECK(sleeping_beauty.done());
}

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

void
timeout(reactor::Signal& s,
        bool expect)
{
  bool finished = wait(s, boost::posix_time::milliseconds(500));
  BOOST_CHECK(finished == expect);
  BOOST_CHECK(s.waiters().empty());
}

void
timeout_send(reactor::Signal& s)
{
  yield();
  BOOST_CHECK_EQUAL(s.waiters().size(), 1);
  s.signal();
}

void
test_timeout_do()
{
  Fixture f;

  reactor::Signal s;
  reactor::Thread t(*sched, "timeout",
                    boost::bind(timeout, boost::ref(s), false));
  sched->run();
}

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

void connor()
{
  reactor::Semaphore s(0);
  reactor::Scheduler::scheduler()->current()->wait(
    s, boost::posix_time::milliseconds(1));
}

void schwarzy()
{
  reactor::Scheduler::scheduler()->terminate();
  ::usleep(10);
}

void test_timeout_aborted()
{
  reactor::Scheduler sched;

  boost::asio::deadline_timer(sched.io_service());
  reactor::Thread t1(sched, "John", &connor);
  reactor::Thread t2(sched, "Terminator", &schwarzy);
  sched.run();
}

/*--------.
| VThread |
`--------*/

int
answer()
{
  return 42;
}

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

void
waker(reactor::Signal& s)
{
  // FIXME: sleeps suck

  // Make sure the scheduler is sleeping.
  sleep(1);
  reactor::Thread w(*sched, "waker",
                    boost::bind(&reactor::Signal::signal, &s));
  // Make sure the scheduler is done.
  sleep(1);
}

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

int
spawned(reactor::Signal& s)
{
  s.signal();
  return 42;
}

void
spawn(reactor::Signal& s,
      int& res)
{
  res = sched->mt_run<int>("spawned", boost::bind(spawned, boost::ref(s)));
}

void
spawner()
{
  reactor::Signal s;
  int res = 0;
  boost::thread spawner(boost::bind(spawn, boost::ref(s), boost::ref(res)));
  wait(s);
  spawner.join();
  BOOST_CHECK_EQUAL(res, 42);
}

void
test_multithread_run()
{
  Fixture f;

  reactor::Thread t(*sched, "spawner", spawner);
  sched->run();
}

/*----------.
| Semaphore |
`----------*/

void
semaphore_noblock_wait(reactor::Semaphore& s)
{
  BOOST_CHECK_EQUAL(s.count(), 2);
  wait(s);
  BOOST_CHECK_EQUAL(s.count(), 1);
  wait(s);
  BOOST_CHECK_EQUAL(s.count(), 0);
}

void
test_semaphore_noblock()
{
  Fixture f;
  reactor::Semaphore s(2);
  reactor::Thread wait(*sched, "wait",
                       boost::bind(&semaphore_noblock_wait, boost::ref(s)));
  sched->run();
}

void
semaphore_block_wait(reactor::Semaphore& s)
{
  BOOST_CHECK_EQUAL(s.count(), 0);
  wait(s);
  BOOST_CHECK_EQUAL(s.count(), 0);
}

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

void
semaphore_multi_wait(reactor::Semaphore& s,
                     int& step)
{
  wait(s);
  ++step;
}

void
semaphore_multi_post(reactor::Semaphore& s,
                     int& step)
{
  yield();
  yield();
  yield();
  BOOST_CHECK_EQUAL(s.count(), -2);
  BOOST_CHECK_EQUAL(step, 0);
  s.release();
  yield();
  BOOST_CHECK_EQUAL(s.count(), -1);
  BOOST_CHECK_EQUAL(step, 1);
  s.release();
  yield();
  BOOST_CHECK_EQUAL(s.count(), 0);
  BOOST_CHECK_EQUAL(step, 2);
}

void
test_semaphore_multi()
{
  Fixture f;
  reactor::Semaphore s;
  int step = 0;
  reactor::Thread wait1(*sched, "wait1",
                       boost::bind(&semaphore_multi_wait,
                                   boost::ref(s), boost::ref(step)));
  reactor::Thread wait2(*sched, "wait2",
                       boost::bind(&semaphore_multi_wait,
                                   boost::ref(s), boost::ref(step)));
  reactor::Thread post(*sched, "post",
                       boost::bind(&semaphore_multi_post,
                                   boost::ref(s), boost::ref(step)));
  sched->run();
}

/*------.
| Mutex |
`------*/

static const int mutex_yields = 32;

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
      reactor::Lock lock(*sched, mutex);
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

void
rw_mutex_read(reactor::RWMutex& mutex,
              int& step)
{
  reactor::Lock lock(*reactor::Scheduler::scheduler(), mutex);
  ++step;
  yield();
  BOOST_CHECK_EQUAL(step, 3);
}

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

void
rw_mutex_write(reactor::RWMutex& mutex,
               int& step)
{
  reactor::Lock lock(*reactor::Scheduler::scheduler(), mutex.write());
  ++step;
  int prev = step;
  yield();
  BOOST_CHECK_EQUAL(step, prev);
}

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

void
rw_mutex_both_read(reactor::RWMutex& mutex,
                   int& step)
{
  reactor::Lock lock(*reactor::Scheduler::scheduler(), mutex);
  int v = step;
  BOOST_CHECK_EQUAL(v % 2, 0);
  BOOST_CHECK_EQUAL(step, v);
  yield();
  BOOST_CHECK_EQUAL(step, v);
  yield();
  BOOST_CHECK_EQUAL(step, v);
}

void
rw_mutex_both_write(reactor::RWMutex& mutex,
                    int& step)
{
  reactor::Lock lock(*reactor::Scheduler::scheduler(), mutex.write());
  ++step;
  yield();
  yield();
  ++step;
  BOOST_CHECK_EQUAL(step % 2, 0);
}

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
#ifdef INFINIT_MACOSX
  for (int i = 0; i < 64; ++i)
#else
  for (int i = 0; i < 128; ++i)
#endif
    threads.push_back(std::thread(action));
  for (auto& thread: threads)
    thread.join();
}

/*------------.
| Multithread |
`------------*/

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

class BeaconException: public elle::Exception
{
public:
  BeaconException()
    : elle::Exception("beacon")
  {}
};

void
except_gen()
{
  throw BeaconException();
}

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

/*-----------------------.
| Terminate now starting |
`-----------------------*/

static
void
test_terminate_now_starting()
{
  bool beacon = true;
  reactor::Scheduler sched;

  reactor::Thread starting(sched, "starting", &empty);
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

void
poster(bool& beacon)
{
  auto& sched = *reactor::Scheduler::scheduler();
  sched.io_service().post(&except_gen);
  sched.current()->yield();
  beacon = true;
}

void
test_io_service_throw()
{
  bool beacon = false;
  reactor::Scheduler sched;
  reactor::Thread thread(sched, "Poster", std::bind(&poster, std::ref(beacon)));
  BOOST_CHECK_THROW(sched.run(), BeaconException);
  BOOST_CHECK(!beacon);
}

/*-----.
| Main |
`-----*/

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
  terminate->add(BOOST_TEST_CASE(test_terminate_now_starting));
  terminate->add(BOOST_TEST_CASE(test_terminate_now_scheduled));
  terminate->add(BOOST_TEST_CASE(test_exception_escape));
  terminate->add(BOOST_TEST_CASE(test_exception_escape_collateral));

  boost::unit_test::test_suite* timeout = BOOST_TEST_SUITE("Timeout");
  boost::unit_test::framework::master_test_suite().add(timeout);
  timeout->add(BOOST_TEST_CASE(test_timeout_do));
  timeout->add(BOOST_TEST_CASE(test_timeout_dont));
  timeout->add(BOOST_TEST_CASE(test_timeout_aborted));

  boost::unit_test::test_suite* vthread = BOOST_TEST_SUITE("Return value");
  boost::unit_test::framework::master_test_suite().add(vthread);
  vthread->add(BOOST_TEST_CASE(test_vthread));

  boost::unit_test::test_suite* mt = BOOST_TEST_SUITE("Multithreading");
  boost::unit_test::framework::master_test_suite().add(mt);
  mt->add(BOOST_TEST_CASE(test_multithread_spawn_wake));
  mt->add(BOOST_TEST_CASE(test_multithread_run));

  boost::unit_test::test_suite* sem = BOOST_TEST_SUITE("Semaphore");
  boost::unit_test::framework::master_test_suite().add(sem);
  sem->add(BOOST_TEST_CASE(test_semaphore_noblock));
  sem->add(BOOST_TEST_CASE(test_semaphore_block));

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

  boost::unit_test::test_suite* thread_exception = BOOST_TEST_SUITE("Thread Exception");
  boost::unit_test::framework::master_test_suite().add(thread_exception);
  storage->add(BOOST_TEST_CASE(thread_exception_test));

  boost::unit_test::test_suite* io_service = BOOST_TEST_SUITE("IO service");
  boost::unit_test::framework::master_test_suite().add(io_service);
  io_service->add(BOOST_TEST_CASE(test_io_service_throw));

  return true;
}

int
main(int argc,
     char** argv)
{
  return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}
