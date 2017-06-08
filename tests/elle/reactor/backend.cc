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
      auto t = m.make_thread("test_die", [&i] { inc(&i); });
      t->step();
      BOOST_TEST(i == 1);
      BOOST_CHECK(t->status() == Thread::Status::done);
    }
    {
      auto t = m.make_thread("test_die", [&i] { inc(&i); });
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

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* backend = BOOST_TEST_SUITE("Backend");
  boost::unit_test::framework::master_test_suite().add(backend);
#if defined(REACTOR_CORO_BACKEND_IO)
# define TEST(Name)                                                     \
  {                                                                     \
    backend->add(                                                       \
      BOOST_TEST_CASE(Name<elle::reactor::backend::coro_io::Backend>),  \
      0, 10);                                                           \
  }
#elif defined(REACTOR_CORO_BACKEND_BOOST_CONTEXT)
# define TEST(Name)                                                          \
  {                                                                          \
    backend->add(                                                            \
      BOOST_TEST_CASE(Name<elle::reactor::backend::boost_context::Backend>), \
      0, 10);                                                                \
  }
#endif
  TEST(test_die);
  TEST(test_deadlock_creation);
  TEST(test_deadlock_switch);
  TEST(test_status);
}
