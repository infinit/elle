#include <elle/test.hh>
#include <elle/system/home_directory.hh>

static
void
size()
{
  BOOST_CHECK(elle::system::home_directory().string().size() > 0);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(size));
}
