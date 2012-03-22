#ifndef INFINIT_REACTOR_TEST_HH
# define INFINIT_REACTOR_TEST_HH

#include <reactor/asio.hh>
#include <boost/foreach.hpp>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <reactor/scheduler.hh>

extern reactor::Scheduler* sched;

class Fixture
{
  public:
    Fixture();
    ~Fixture();
};

void empty();

#endif
