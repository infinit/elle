#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <elle/container/set.hh>

#include <reactor/scheduler.hh>
#include <reactor/signal.hh>
#include <reactor/fsm.hh>

using reactor::fsm::Machine;
using reactor::fsm::State;
using reactor::fsm::Transition;

class BeaconException:
  public elle::Exception
{
public:
  BeaconException():
    elle::Exception("beacon")
  {}
};

static
void
test_model()
{
  Machine m;
  State& s1 = m.state_make();
  State& s2 = m.state_add(std::unique_ptr<State>(new State()));

  reactor::Signal s;

  Transition& t = m.transition_add(s1, s2, reactor::Waitables{&s});

  BOOST_CHECK_EQUAL(m.start(), &s1);
  BOOST_CHECK_EQUAL(s1.transitions_out().size(), 1);
  BOOST_CHECK_EQUAL(s1.transitions_in().size(), 0);
  BOOST_CHECK(contains(s1.transitions_out(), &t));
  BOOST_CHECK_EQUAL(s2.transitions_out().size(), 0);
  BOOST_CHECK_EQUAL(s2.transitions_in().size(), 1);
  BOOST_CHECK(contains(s2.transitions_in(), &t));
}

static
void
test_run_one_state()
{
  Machine m;
  bool beacon = false;
  m.state_make([&] () { beacon = true; });

  reactor::Scheduler sched;
  reactor::Thread run(sched, "run", std::bind(&Machine::run, &m));
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

  reactor::Scheduler sched;
  reactor::Thread run(sched, "run", std::bind(&Machine::run, &m));
  sched.run();
  BOOST_CHECK(beacon1);
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
  reactor::Signal trigger;
  m.transition_add(s1, s2, reactor::Waitables{&trigger});

  reactor::Scheduler sched;
  reactor::Thread run(sched, "run", std::bind(&Machine::run, &m));
  reactor::Thread check(sched, "check", [&](){
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
test_run_unused_transition()
{
  Machine m;

  State& s1 = m.state_make("start");
  State& s2 = m.state_make("end");
  m.transition_add(s1, s2);
  reactor::Signal trigger;
  m.transition_add(s1, s2, reactor::Waitables{&trigger});

  reactor::Scheduler sched;
  reactor::Thread run(sched, "run", std::bind(&Machine::run, &m));
  sched.run();
}

static
void
test_run_preemptive_transition(bool preemptive)
{
  reactor::Scheduler sched;
  Machine m;

  bool beacon1 = false;
  bool beacon2 = false;
  bool beacon3 = false;

  reactor::Signal trigger_state;
  State& s1 = m.state_make("start", [&](){
      auto& current = *sched.current();
      beacon1 = true;
      current.wait(trigger_state);
      beacon2 = true;
    });
  State& s2 = m.state_make("end", [&] () { beacon3 = true; });
  reactor::Signal trigger;
  m.transition_add(s1, s2, reactor::Waitables{&trigger}, preemptive);

  reactor::Thread run(sched, "run", std::bind(&Machine::run, &m));
  reactor::Thread check(sched, "check", [&](){
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

static
void
test_run_two_transitions_triggered(bool order)
{
  reactor::Scheduler sched;
  Machine m;

  bool beacon1 = false;
  bool beacon2 = false;
  bool beacon3 = false;

  reactor::Signal trigger_state;
  State& s1 = m.state_make("start", [&] () { beacon1 = true; });
  State& s2 = m.state_make("end_2", [&] () { beacon2 = true; });
  State& s3 = m.state_make("end_3", [&] () { beacon3 = true; });
  reactor::Signal trigger2;
  m.transition_add(s1, s2, reactor::Waitables{&trigger2});
  reactor::Signal trigger3;
  m.transition_add(s1, s3, reactor::Waitables{&trigger3});

  reactor::Thread run(sched, "run", std::bind(&Machine::run, &m));
  reactor::Thread check(sched, "check", [&](){
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
test_exception()
{
  reactor::Scheduler sched;
  Machine m;

  m.state_make("start", [&] () { throw BeaconException(); });

  reactor::Thread run(sched, "run", std::bind(&Machine::run, &m));
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
      auto& current = *reactor::Scheduler::scheduler()->current();
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
  reactor::Signal trigger;
  m.transition_add(s1, s3, reactor::Waitables{&trigger});

  reactor::Scheduler sched;
  reactor::Thread run(sched, "run", std::bind(&Machine::run, &m));
  reactor::Thread trigger_thread(sched, "trigger", [&] () {
      auto& current = *reactor::Scheduler::scheduler()->current();
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

  State& s1 = m.state_make([&] () {
      beacon1 = true;
      throw BeaconException();
    });
  State& s2 = m.state_make([&] () {
      beacon2 = true;
    });
  State& s3 = m.state_make([&] () {
      beacon3 = true;
    });
  m.transition_add(s1, s2);
  m.transition_add_catch(s1, s3);

  reactor::Scheduler sched;
  reactor::Thread run(sched, "run", std::bind(&Machine::run, &m));
  sched.run();
  BOOST_CHECK(beacon1);
  BOOST_CHECK(!beacon2);
  BOOST_CHECK(beacon3);
}

static
bool
test_suite()
{
  boost::unit_test::test_suite* fsm = BOOST_TEST_SUITE("fsm");
  boost::unit_test::framework::master_test_suite().add(fsm);
  fsm->add(BOOST_TEST_CASE(test_model));
  fsm->add(BOOST_TEST_CASE(test_run_one_state));
  fsm->add(BOOST_TEST_CASE(test_run_two_states));
  fsm->add(BOOST_TEST_CASE(test_run_transition_signal));
  fsm->add(BOOST_TEST_CASE(test_run_unused_transition));
  fsm->add(BOOST_TEST_CASE(std::bind(test_run_preemptive_transition, false)));
  fsm->add(BOOST_TEST_CASE(std::bind(test_run_preemptive_transition, true)));
  fsm->add(BOOST_TEST_CASE(std::bind(test_run_two_transitions_triggered, false)));
  fsm->add(BOOST_TEST_CASE(std::bind(test_run_two_transitions_triggered, true)));
  fsm->add(BOOST_TEST_CASE(test_exception));
  fsm->add(BOOST_TEST_CASE(transition_auto_versus_waitable));
  fsm->add(BOOST_TEST_CASE(transition_catch));
  return true;
}

int
main(int argc, char** argv)
{
  return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}
