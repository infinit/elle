#ifndef INFINIT_REACTOR_TEST_HH
# define INFINIT_REACTOR_TEST_HH

#include <reactor/asio.hh>
#include <reactor/scheduler.hh>

#include <boost/foreach.hpp>

extern reactor::Scheduler* sched;

class Fixture
{
  public:
    Fixture();
    ~Fixture();
};

void
empty();

#endif
