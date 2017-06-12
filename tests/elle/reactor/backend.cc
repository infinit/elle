#include <elle/test.hh>

#if defined(REACTOR_CORO_BACKEND_IO)
# include <elle/reactor/backend/coro_io/backend.hh>
#elif defined(REACTOR_CORO_BACKEND_BOOST_CONTEXT)
# include <elle/reactor/backend/boost_context/backend.hh>
#endif

#include <boost/bind.hpp>

using elle::reactor::backend::Thread;

namespace
{
  void
  empty()
  {}

  void
  inc(int* i)
  {
    ++*i;
  }

  template <typename Backend>
  void
  test_die()
  {
    auto&& m = Backend{};
    int i = 0;
    {
      auto t = m.make_thread("test_die1", [&i] { inc(&i); });
      t->step();
      BOOST_TEST(i == 1);
      BOOST_CHECK(t->status() == Thread::Status::done);
    }
    {
      auto t = m.make_thread("test_die2", [&i] { inc(&i); });
      t->step();
      BOOST_TEST(i == 2);
      BOOST_CHECK(t->status() == Thread::Status::done);
    }
  }

  template <typename Backend>
  void
  test_deadlock_creation()
  {
    auto&& m = Backend{};
    auto t = m.make_thread("test_deadlock_creation", empty);
    t->step();
    BOOST_CHECK(t->status() == Thread::Status::done);
  }

  template <typename Backend>
  void
  test_deadlock_switch()
  {
    auto&& m = Backend{};

    auto one_yield = [&m]
      {
        m.current()->yield();
      };

    auto t = m.make_thread("test_deadlock_switch", one_yield);
    t->step();
    t->step();
    BOOST_CHECK(t->status() == Thread::Status::done);
  }

  template <typename Backend>
  void
  test_status()
  {
    auto&& m = Backend{};

    auto status_coro = [&m]
    {
      BOOST_CHECK(m.current()->status() == Thread::Status::running);
      m.current()->yield();
      BOOST_CHECK(m.current()->status() == Thread::Status::running);
    };

    auto t = m.make_thread("status", status_coro);
    BOOST_CHECK(t->status() == Thread::Status::starting);
    t->step();
    BOOST_CHECK(t->status() == Thread::Status::waiting);
    t->step();
    BOOST_CHECK(t->status() == Thread::Status::done);
  }
}

#define BOOST_NAMED_TEST_CASE(Name, Function)   \
  boost::unit_test::make_test_case(             \
    Function, Name, __FILE__, __LINE__ )

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* backend = BOOST_TEST_SUITE("Backend");
  boost::unit_test::framework::master_test_suite().add(backend);

#if defined REACTOR_CORO_BACKEND_IO
  using Backend = elle::reactor::backend::coro_io::Backend;
#elif defined REACTOR_CORO_BACKEND_BOOST_CONTEXT
  using Backend = elle::reactor::backend::boost_context::Backend;
#endif

#define TEST(Name)                                                     \
  backend->add(BOOST_NAMED_TEST_CASE(#Name, test_ ## Name<Backend>), 0, 10)

  TEST(die);
  TEST(deadlock_creation);
  TEST(deadlock_switch);
  TEST(status);
}
