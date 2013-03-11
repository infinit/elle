#include <boost/bind.hpp>

#include <reactor/backend/thread.hh>

#include "../test.hh"

using reactor::backend::Manager;
using reactor::backend::Thread;

Manager* m = 0;

static void one_yield()
{
  m->current()->yield();
}

static void inc(int* i)
{
  ++*i;
}

static void test_die()
{
  m = new Manager;
  int i = 0;
  {
    Thread t(*m, "test_die", boost::bind(inc, &i));
    t.step();
    BOOST_CHECK_EQUAL(i, 1);
    BOOST_CHECK(t.done());
  }
  {
    Thread t(*m, "test_die", boost::bind(inc, &i));
    t.step();
    BOOST_CHECK_EQUAL(i, 2);
    BOOST_CHECK(t.done());
  }
  delete m;
}

static void test_deadlock_creation()
{
  m = new Manager;

  Thread t(*m, "test_deadlock_creation", empty);
  t.step();
  BOOST_CHECK(t.done());
  delete m;
}

static void test_deadlock_switch()
{
  m = new Manager;
  Thread t(*m, "test_deadlock_switch", one_yield);
  t.step();
  t.step();
  BOOST_CHECK(t.done());
  delete m;
}

using namespace reactor::backend::status;

static void status_coro()
{
  BOOST_CHECK(m->current()->status() == running);
  m->current()->yield();
  BOOST_CHECK(m->current()->status() == running);
}

static void test_status()
{
  m = new Manager;
  Thread t(*m, "status", status_coro);
  BOOST_CHECK(t.status() == starting);
  t.step();
  BOOST_CHECK(t.status() == waiting);
  t.step();
  BOOST_CHECK(t.status() == done);
  BOOST_CHECK(t.done());
  delete m;
}

namespace reactor
{
  namespace backend
  {
    boost::unit_test::test_suite* test_suite()
    {
      boost::unit_test::test_suite* backend = BOOST_TEST_SUITE("Backend");
      backend->add(BOOST_TEST_CASE(test_die));
      backend->add(BOOST_TEST_CASE(test_deadlock_creation));
      backend->add(BOOST_TEST_CASE(test_deadlock_switch));
      backend->add(BOOST_TEST_CASE(test_status));
      return backend;
    }
  }
}
