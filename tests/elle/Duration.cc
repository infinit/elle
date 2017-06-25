#include <iostream>
#include <sstream>

#include <elle/test.hh>

#include <elle/Duration.hh>

using namespace std::literals;

namespace
{
  elle::Duration id(elle::Duration d = 30s)
  {
    return d;
  }

  elle::Duration optid(elle::DurationOpt d = 30s)
  {
    return *d;
  }

  void
  optional()
  {
    auto s = elle::Duration{1s};
    BOOST_TEST(s == 1000ms);

    BOOST_TEST(elle::Duration{1ms} == 1ms);
    BOOST_TEST(*elle::DurationOpt{1ms} == 1ms);

    BOOST_TEST(elle::Duration{30s} == 30s);
    BOOST_TEST(*elle::DurationOpt{30s} == 30s);
    BOOST_TEST(id() == 30s);
    BOOST_TEST(optid() == 30s);
  }
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(optional), 0, 1);
}
