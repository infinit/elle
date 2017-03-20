#include <elle/reactor/asio.hh>
#include <elle/reactor/Barrier.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/reactor/signal.hh>
#include <elle/reactor/fsm.hh>

#include <elle/test.hh>

using elle::reactor::fsm::Machine;
using elle::reactor::fsm::State;
using elle::reactor::fsm::Transition;

class BeaconException:
  public elle::Exception
{
public:
  BeaconException(int value = 0):
    elle::Exception("beacon"),
    _value(value)
  {}
private:
  ELLE_ATTRIBUTE_R(int, value);
};

class OtherException:
  public elle::Exception
{
public:
  OtherException():
    elle::Exception("other")
  {}
};

static
void
test_model()
{
  Machine m;
  State& s1 = m.state_make();
  State& s2 = m.state_add(std::unique_ptr<State>(new State()));

  elle::reactor::Signal s;

  Transition& t = m.transition_add(s1, s2, elle::reactor::Waitables{&s});

  BOOST_CHECK_EQUAL(m.start(), &s1);
  BOOST_CHECK_EQUAL(s1.transitions_out().size(), 1);
  BOOST_CHECK_EQUAL(s1.transitions_in().size(), 0);

  BOOST_CHECK(std::find(s1.transitions_out().begin(),
                        s1.transitions_out().end(),
                        &t) != s1.transitions_out().end());

  BOOST_CHECK_EQUAL(s2.transitions_out().size(), 0);
  BOOST_CHECK_EQUAL(s2.transitions_in().size(), 1);

  BOOST_CHECK(std::find(s2.transitions_in().begin(),
                        s2.transitions_in().end(),
                        &t) != s2.transitions_in().end());

}

static
void
test_run_one_state()
{
  Machine m;
  bool beacon = false;
  m.state_make([&] () { beacon = true; });

  elle::reactor::Scheduler sched;
  elle::reactor::Thread run(sched, "run", [&] { m.run(); });
  sched.run();

  BOOST_CHECK(beacon);
  BOOST_CHECK(!m.running());
}

static
void
test_run_two_states()
{
  Machine m;

  bool beacon1 = false;
  bool beacon2 = false;

  State& s1 = m.state_make([&] () {
      BOOST_CHECK(!beacon1);
      BOOST_CHECK(!beacon2);
      beacon1 = true;
    });
  State& s2 = m.state_make([&] () {
      BOOST_CHECK(beacon1);
      BOOST_CHECK(!beacon2);
      beacon2 = true;
    });
  m.transition_add(s1, s2);

  elle::reactor::Scheduler sched;
  elle::reactor::Thread run(sched, "run", [&] { m.run(); });
  sched.run();
  BOOST_CHECK(beacon1);
  BOOST_CHECK(beacon2);
}

static
void
test_run_start_state()
{
  Machine m;

  bool beacon1 = false;
  bool beacon2 = false;

  State& s1 = m.state_make([&] () {
      BOOST_CHECK(!beacon1);
      BOOST_CHECK(!beacon2);
      beacon1 = true;
    });
  State& s2 = m.state_make([&] () {
      BOOST_CHECK(!beacon1);
      BOOST_CHECK(!beacon2);
      beacon2 = true;
    });
  m.transition_add(s1, s2);

  elle::reactor::Scheduler sched;
  elle::reactor::Thread run(sched, "run", [&] { m.run(s2); });
  sched.run();
  BOOST_CHECK(!beacon1);
  BOOST_CHECK(beacon2);
}

static
void
test_run_transition_signal()
{
  Machine m;

  bool beacon1 = false;
  bool beacon2 = false;
  bool beacon3 = false;

  State& s1 = m.state_make("start", [&] () {
      beacon1 = true;
    });
  State& s2 = m.state_make("end", [&] () {
      beacon2 = true;
    });
  elle::reactor::Signal trigger;
  m.transition_add(s1, s2, elle::reactor::Waitables{&trigger});

  elle::reactor::Scheduler sched;
  elle::reactor::Thread run(sched, "run", [&] { m.run(); });
  elle::reactor::Thread check(sched, "check", [&](){
      auto& current = *sched.current();
      current.wait(s1.done());
      BOOST_CHECK(beacon1);
      BOOST_CHECK(!current.wait(s1.exited(), 500_ms));
      BOOST_CHECK(!beacon2);
      trigger.signal();
      current.wait(s2.exited());
      BOOST_CHECK(beacon2);
      beacon3 = true;
    });
  sched.run();
  BOOST_CHECK(beacon1);
  BOOST_CHECK(beacon2);
  BOOST_CHECK(beacon3);
}

static
void
test_run_transition_pre_trigger()
{
  Machine m;

  bool beacon1 = false;
  bool beacon2 = false;
  bool beacon3 = false;

  State& s1 = m.state_make([&] {beacon1 = true;});
  State& s2 = m.state_make([&] {beacon2 = true;});
  State& s3 = m.state_make([&] {beacon3 = true;});
  elle::reactor::Signal trigger1;
  m.transition_add(s1, s2,
                   elle::reactor::Waitables{&trigger1}, false, [] { return false; });
  elle::reactor::Signal trigger2;
  m.transition_add(s1, s3,
                   elle::reactor::Waitables{&trigger2}, false, [] { return true; });

  elle::reactor::Scheduler sched;
  elle::reactor::Thread run(sched, "run", [&] { m.run(); });
  sched.run();
  BOOST_CHECK(beacon1);
  BOOST_CHECK(!beacon2);
  BOOST_CHECK(beacon3);
}

static
void
test_run_unused_transition()
{
  Machine m;

  State& s1 = m.state_make("start");
  State& s2 = m.state_make("end");
  m.transition_add(s1, s2);
  elle::reactor::Signal trigger;
  m.transition_add(s1, s2, elle::reactor::Waitables{&trigger});

  elle::reactor::Scheduler sched;
  elle::reactor::Thread run(sched, "run", [&] { m.run(); });
  sched.run();
}

static
void
test_run_preemptive_transition(bool preemptive)
{
  elle::reactor::Scheduler sched;
  Machine m;

  bool beacon1 = false;
  bool beacon2 = false;
  bool beacon3 = false;

  elle::reactor::Signal trigger_state;
  State& s1 = m.state_make("start", [&](){
      auto& current = *sched.current();
      beacon1 = true;
      current.wait(trigger_state);
      beacon2 = true;
    });
  State& s2 = m.state_make("end", [&] () { beacon3 = true; });
  elle::reactor::Signal trigger;
  m.transition_add(s1, s2, elle::reactor::Waitables{&trigger}, preemptive);

  elle::reactor::Thread run(sched, "run", [&] { m.run(); });
  elle::reactor::Thread check(sched, "check", [&](){
      auto& current = *sched.current();
      // Make sure we're way into the FSM.
      current.yield();
      current.yield();
      trigger.signal();
      current.yield();
      trigger_state.signal();
    });

  sched.run();
  BOOST_CHECK(beacon1);
  BOOST_CHECK(beacon2 == !preemptive);
  BOOST_CHECK(beacon3);
}

static void
test_run_preemptive_transition_pre_opened()
{
  elle::reactor::Scheduler sched;
  Machine m;
  elle::reactor::Barrier trigger;

  bool beacon1 = false;
  bool beacon2 = false;
  bool beacon3 = false;

  State& s1 = m.state_make("s1", [&] () { beacon1 = true; trigger.open(); });
  State& s2 = m.state_make("s2", [&] () { beacon2 = true; });
  State& s3 = m.state_make("s3", [&] () { beacon3 = true; });
  m.transition_add(s1, s2, elle::reactor::Waitables{&trigger}, true);
  m.transition_add(s2, s3, elle::reactor::Waitables{&trigger}, true);

  elle::reactor::Thread run(sched, "run", [&] { m.run(); });
  sched.run();

  BOOST_CHECK(beacon1);
  BOOST_CHECK(!beacon2);
  BOOST_CHECK(beacon3);
}

static
void
test_run_two_transitions_triggered(bool order)
{
  elle::reactor::Scheduler sched;
  Machine m;

  bool beacon1 = false;
  bool beacon2 = false;
  bool beacon3 = false;

  elle::reactor::Signal trigger_state;
  State& s1 = m.state_make("start", [&] () { beacon1 = true; });
  State& s2 = m.state_make("end_2", [&] () { beacon2 = true; });
  State& s3 = m.state_make("end_3", [&] () { beacon3 = true; });
  elle::reactor::Signal trigger2;
  m.transition_add(s1, s2, elle::reactor::Waitables{&trigger2});
  elle::reactor::Signal trigger3;
  m.transition_add(s1, s3, elle::reactor::Waitables{&trigger3});

  elle::reactor::Thread run(sched, "run", [&] { m.run(); });
  elle::reactor::Thread check(sched, "check", [&](){
      auto& current = *sched.current();
      // Make sure we're way into the FSM.
      current.yield();
      current.yield();
      if (order)
      {
        trigger2.signal();
        current.yield();
        trigger3.signal();
      }
      else
      {
        trigger3.signal();
        current.yield();
        trigger2.signal();
      }
    });

  sched.run();
  BOOST_CHECK(beacon1);
  BOOST_CHECK(beacon2 == order);
  BOOST_CHECK(beacon3 == !order);
}

static
void
test_recursive_state()
{
  elle::reactor::Scheduler sched;
  Machine m;

  int beacon1 = 0;
  bool beacon2 = false;

  State& s1 = m.state_make("start", [&] () { ++beacon1;
                                             if(beacon1 == 10)
                                               throw BeaconException(); });
  State& end = m.state_make("end", [&] () { beacon2 = true; });

  m.transition_add_catch(s1, end);
  m.transition_add(s1, s1);

  elle::reactor::Thread run(sched, "run", [&] { m.run(); });

  sched.run();
  BOOST_CHECK_EQUAL(beacon1, 10);
  BOOST_CHECK(beacon2);
}

static
void
test_circular()
{
  elle::reactor::Scheduler sched;
  Machine m;

  int beacon1 = 0;
  int beacon2 = 0;
  int beacon3 = 0;
  bool beacon4 = true;

  State& s1 = m.state_make("start", [&] () { ++beacon1; });
  State& s2 = m.state_make("s2", [&] () { ++beacon2; });
  State& s3 = m.state_make("s3", [&] () { ++beacon3;
                                          if(beacon3 == 3)
                                            throw BeaconException(); });
  State& end = m.state_make("end", [&] () { beacon4 = true; });

  m.transition_add(s1, s2);
  m.transition_add(s2, s3);
  m.transition_add(s3, s1);
  m.transition_add_catch(s3, end);

  elle::reactor::Thread run(sched, "run", [&] { m.run(); });

  sched.run();
  BOOST_CHECK_EQUAL(beacon1, 3);
  BOOST_CHECK_EQUAL(beacon2, 3);
  BOOST_CHECK_EQUAL(beacon3, 3);
  BOOST_CHECK(beacon4);
}

// FIFT stands for First In, First Triggered.
static
void
test_FIFT()
{
  elle::reactor::Scheduler sched;
  Machine m;

  bool beacon1 = false;
  bool beacon2 = false;

  State& s1 = m.state_make("start", [&] () { beacon1 = true; });
  State& s2 = m.state_make("end", [&] () { beacon2 = true; });
  State& forbidden1 = m.state_make("forbidden1",
                                  [&] ()
                                  {
                                    BOOST_CHECK(false);
                                  });
  State& forbidden2 = m.state_make("forbidden2",
                                  [&] ()
                                  {
                                    BOOST_CHECK(false);
                                  });
  State& forbidden3 = m.state_make("forbidden3",
                                  [&] ()
                                  {
                                    BOOST_CHECK(false);
                                  });

  m.transition_add_catch(s1, forbidden3);
  m.transition_add(s1, s2);
  m.transition_add(s1, forbidden1);
  m.transition_add(s1, forbidden2);

  elle::reactor::Thread run(sched, "run", [&] { m.run(); });

  sched.run();
  BOOST_CHECK(beacon1);
  BOOST_CHECK(beacon2);
}

static
void
test_exception()
{
  elle::reactor::Scheduler sched;
  Machine m;

  m.state_make("start", [&] () { throw BeaconException(); });

  elle::reactor::Thread run(sched, "run", [&] { m.run(); });
  BOOST_CHECK_THROW(sched.run(), BeaconException);
}

static
void
transition_auto_versus_waitable()
{
  Machine m;

  bool beacon1 = false;
  bool beacon2 = false;
  bool beacon3 = false;

  State& s1 = m.state_make([&] () {
      beacon1 = true;
      auto& current = *elle::reactor::Scheduler::scheduler()->current();
      current.yield();
      current.yield();
      current.yield();
      current.yield();
    });
  State& s2 = m.state_make([&] () {
      beacon2 = true;
    });
  State& s3 = m.state_make([&] () {
      beacon3 = true;
    });
  m.transition_add(s1, s2);
  elle::reactor::Signal trigger;
  m.transition_add(s1, s3, elle::reactor::Waitables{&trigger});

  elle::reactor::Scheduler sched;
  elle::reactor::Thread run(sched, "run", [&] { m.run(); });
  elle::reactor::Thread trigger_thread(sched, "trigger", [&] () {
      auto& current = *elle::reactor::Scheduler::scheduler()->current();
      current.yield();
      current.yield();
      trigger.signal();
    });
  sched.run();
  BOOST_CHECK(beacon1);
  BOOST_CHECK(!beacon2);
  BOOST_CHECK(beacon3);
}

static
void
transition_catch()
{
  Machine m;

  bool beacon1 = false;
  bool beacon2 = false;
  bool beacon3 = false;
  int caught = -1;

  State& s1 = m.state_make([&] () {
      beacon1 = true;
      throw BeaconException(42);
    });
  State& s2 = m.state_make([&] () {
      beacon2 = true;
    });
  State& s3 = m.state_make([&] () {
      beacon3 = true;
    });
  m.transition_add(s1, s2);
  m.transition_add_catch(s1, s3).action_exception(
    [&caught] (std::exception_ptr e)
    {
      try
      {
        std::rethrow_exception(e);
      }
      catch (BeaconException const& e)
      {
        caught = e.value();
      }
    });

  elle::reactor::Scheduler sched;
  elle::reactor::Thread run(sched, "run", [&] { m.run(); });
  sched.run();
  BOOST_CHECK(beacon1);
  BOOST_CHECK(!beacon2);
  BOOST_CHECK(beacon3);
  BOOST_CHECK_EQUAL(caught, 42);
}

static
void
transition_catch_specific()
{
  Machine m;

  bool beacon1 = false;
  bool beacon2 = false;
  bool beacon3 = false;

  State& s1 = m.state_make(
    "s1",
    [&] ()
    {
      beacon1 = true;
      throw BeaconException();
    });
  State& s2 = m.state_make(
    "s2",
    [&] ()
    {
      beacon2 = true;
      throw BeaconException();
    });
  State& s3 = m.state_make(
    "s3",
    [&] ()
    {
      beacon3 = true;
      throw BeaconException();
    });
  State& forbidden = m.state_make(
    "forbidden",
    [&] ()
    {
      BOOST_CHECK(false);
    });
  // Check we follow the right exception
  m.transition_add_catch_specific<OtherException>(s1, forbidden);
  m.transition_add_catch_specific<BeaconException>(s1, s2);
  m.transition_add_catch(s1, forbidden);

  // Check we follow the default.
  m.transition_add_catch_specific<OtherException>(s2, forbidden);
  m.transition_add_catch(s2, s3);

  // Check the exception escapes.
  m.transition_add_catch_specific<OtherException>(s3, forbidden);

  elle::reactor::Scheduler sched;
  elle::reactor::Thread run(sched, "run", [&] { m.run(); });
  BOOST_CHECK_THROW(sched.run(), BeaconException);
  BOOST_CHECK(beacon1);
  BOOST_CHECK(beacon2);
  BOOST_CHECK(beacon3);
}

static
void
transition_catch_priority()
{
  Machine m;
  bool beacon1, beacon2, beacon3;
  auto reset_beacons = [&] {
    beacon1 = false;
    beacon2 = false;
    beacon3 = false;
  };
  reset_beacons();
  State& s1 = m.state_make(
    "s1",
    [&] ()
    {
      beacon1 = true;
      throw BeaconException();
    });
  State& s2 = m.state_make(
    "s2",
    [&] ()
    {
      beacon2 = true;
    });
  State& s3 = m.state_make(
    "s3",
    [&] ()
    {
      beacon3 = true;
    });
  m.transition_add_catch(s1, s2);
  m.transition_add_catch(s1, s3);
  auto run = [&] {
    elle::reactor::Scheduler sched;
    elle::reactor::Thread run(sched, "run", [&] { m.run(); });
    sched.run();
  };
  run();
  BOOST_CHECK(beacon1);
  BOOST_CHECK(beacon2);
  BOOST_CHECK(!beacon3);
  // The transition is specific exception will be 'hidden' by the global catch.
  reset_beacons();
  bool handle_first = false;
  m.transition_add_catch_specific<BeaconException>(s1, s3, handle_first);
  run();
  BOOST_CHECK(beacon1);
  BOOST_CHECK(beacon2);
  BOOST_CHECK(!beacon3);
  // Now, the transition will be treated first.
  reset_beacons();
  handle_first = true;
  m.transition_add_catch_specific<BeaconException>(s1, s3, handle_first);
  run();
  BOOST_CHECK(beacon1);
  BOOST_CHECK(!beacon2);
  BOOST_CHECK(beacon3);
}

static
void
transition_catch_terminate()
{
  elle::reactor::Scheduler sched;
  Machine m;

  bool beacon1 = false;
  bool beacon2 = false;

  State& s1 = m.state_make([&] () {
      beacon1 = true;
      sched.terminate();
    });
  State& s2 = m.state_make([&] () {
      beacon2 = true;
    });
  m.transition_add_catch(s1, s2);

  elle::reactor::Thread run(sched, "run", [&] { m.run(); });
  sched.run();
  BOOST_CHECK(beacon1);
  BOOST_CHECK(!beacon2);
}

static
void
transition_actions()
{
  Machine m;

  bool beacon1 = false;
  bool beacon2 = false;

  State& s1 = m.state_make([&] () {
      BOOST_CHECK(!beacon1);
      BOOST_CHECK(!beacon2);
    });
  State& s2 = m.state_make([&] () {
      BOOST_CHECK(beacon1);
      BOOST_CHECK(!beacon2);
    });
  State& s3 = m.state_make([&] () {
      BOOST_CHECK(beacon1);
      BOOST_CHECK(beacon2);
    });
  m.transition_add(s1, s2).action([&] () { beacon1 = true; });
  m.transition_add(s2, s3).action([&] () { beacon2 = true; });

  elle::reactor::Scheduler sched;
  elle::reactor::Thread run(sched, "run", [&] { m.run(); });
  sched.run();
  BOOST_CHECK(beacon1);
  BOOST_CHECK(beacon2);
}

static
void
transition_action_throw()
{
  Machine m;

  bool beacon1 = false;
  bool beacon2 = false;
  bool beacon3 = false;
  bool beacon4 = false;

  State& s1 = m.state_make([&] { beacon1 = true; });
  State& s2 = m.state_make([&] { beacon2 = true; });
  State& s3 = m.state_make([&] { beacon3 = true; });
  State& s4 = m.state_make([&] { beacon4 = true; });
  m.transition_add(s1, s2).action([&] () { throw BeaconException(); });
  m.transition_add_catch(s1, s3);
  m.transition_add(s3, s4).action([&] () { throw BeaconException(); });

  elle::reactor::Scheduler sched;
  elle::reactor::Thread run(sched, "run", [&] { m.run(); });
  BOOST_CHECK_THROW(sched.run(), BeaconException);
  BOOST_CHECK(beacon1);
  BOOST_CHECK(!beacon2);
  BOOST_CHECK(beacon3);
  BOOST_CHECK(!beacon4);
}

// Check state can't trigger transition before they add the time to actually
// wait on their event.
static
void
transition_give_time()
{
  Machine m;

  elle::reactor::Signal trigger;

  State& s1 = m.state_make([&] { trigger.signal(); });
  State& s2 = m.state_make();
  m.transition_add(s1, s2, elle::reactor::Waitables{&trigger});

  elle::reactor::Scheduler sched;
  elle::reactor::Thread run(sched, "run", [&] { m.run(); });
  sched.run();
}

// Check the Machine terminates its current state before destroying it.
static
void
terminate()
{
  elle::reactor::Scheduler sched;
  Machine m;

  elle::reactor::Thread run(sched, "run", [&] { m.run(); });
  m.state_make(
    [&]
    {
      sched.terminate();
      elle::reactor::Scheduler::scheduler()->current()->yield();
      elle::reactor::Scheduler::scheduler()->current()->yield();
      elle::reactor::Scheduler::scheduler()->current()->yield();
      elle::reactor::Scheduler::scheduler()->current()->yield();
    });

  sched.run();
}

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* fsm = BOOST_TEST_SUITE("fsm");
  boost::unit_test::framework::master_test_suite().add(fsm);
  fsm->add(BOOST_TEST_CASE(test_model), 0, 10);
  fsm->add(BOOST_TEST_CASE(test_run_one_state), 0, 10);
  fsm->add(BOOST_TEST_CASE(test_run_two_states), 0, 10);
  fsm->add(BOOST_TEST_CASE(test_run_start_state), 0, 10);
  fsm->add(BOOST_TEST_CASE(test_run_transition_signal), 0, 10);
  fsm->add(BOOST_TEST_CASE(test_run_transition_pre_trigger), 0, 10);
  fsm->add(BOOST_TEST_CASE(test_run_unused_transition), 0, 10);
  fsm->add(BOOST_TEST_CASE([] { test_run_preemptive_transition(false); }), 0, 10);
  fsm->add(BOOST_TEST_CASE([] { test_run_preemptive_transition(true); }), 0, 10);
  fsm->add(BOOST_TEST_CASE(test_run_preemptive_transition_pre_opened), 0, 10);
  fsm->add(BOOST_TEST_CASE([] { test_run_two_transitions_triggered(false); }), 0, 10);
  fsm->add(BOOST_TEST_CASE([] { test_run_two_transitions_triggered(true); }), 0, 10);
  fsm->add(BOOST_TEST_CASE(test_recursive_state), 0, 10);
  fsm->add(BOOST_TEST_CASE(test_circular), 0, 10);
  fsm->add(BOOST_TEST_CASE(test_FIFT), 0, 10);
  fsm->add(BOOST_TEST_CASE(test_exception), 0, 10);
  fsm->add(BOOST_TEST_CASE(transition_auto_versus_waitable), 0, 10);
  fsm->add(BOOST_TEST_CASE(transition_catch), 0, 10);
  fsm->add(BOOST_TEST_CASE(transition_catch_specific), 0, 10);
  fsm->add(BOOST_TEST_CASE(transition_catch_priority), 0, 10);
  fsm->add(BOOST_TEST_CASE(transition_catch_terminate), 0, 10);
  fsm->add(BOOST_TEST_CASE(transition_actions), 0, 10);
  fsm->add(BOOST_TEST_CASE(transition_action_throw), 0, 10);
  fsm->add(BOOST_TEST_CASE(transition_give_time), 0, 10);
  fsm->add(BOOST_TEST_CASE(terminate), 0, 10);
}
