#ifndef INFINIT_REACTOR_TEST_HH
# define INFINIT_REACTOR_TEST_HH

#include <elle/reactor/asio.hh>
#include <elle/reactor/scheduler.hh>

#include <boost/foreach.hpp>


class Fixture
{
  public:
    Fixture();
    ~Fixture();
};

void
empty();

#endif
