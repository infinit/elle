#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <elle/container/set.hh>

#include <reactor/scheduler.hh>
#include <reactor/signal.hh>
#include <reactor/fsm.hh>

using reactor::fsm::Machine;
using reactor::fsm::State;
using reactor::fsm::Transition;

static
void
test_model()
{
  Machine m;
  State& s1 = m.state_make();
  State& s2 = m.state_add
    (std::unique_ptr<State>(new State()));

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
  return true;
}

int
main(int argc, char** argv)
{
  return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}
