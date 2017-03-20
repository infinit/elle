#include <elle/network/hostname.hh>
#include <elle/test.hh>

static
void
hostname()
{
  char name[1024];
  gethostname(name, sizeof(name));
  BOOST_CHECK_EQUAL(elle::network::hostname(), name);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();

  suite.add(BOOST_TEST_CASE(hostname));
}
