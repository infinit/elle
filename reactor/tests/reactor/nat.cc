#define BOOST_TEST_MODULE nat
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <reactor/network/nat.hh>
#include <reactor/scheduler.hh>

#include <elle/print.hh>

using namespace reactor;
using namespace reactor::network;

BOOST_AUTO_TEST_CASE(nattest)
{
  Scheduler sched;

  auto test_fn = [&]
  {
    reactor::nat::NAT n{sched};

    auto hole = n.punch("infinit.im", 9999, 8345);
    BOOST_CHECK_NE(hole.public_endpoint().port(), 0);
  };
  Thread t(sched, "test", test_fn);
  sched.run();
}
