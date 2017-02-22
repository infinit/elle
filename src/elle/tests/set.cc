#include <set>

#include <elle/test.hh>

static
void
contains()
{
  std::set<int> set({{0, 2}});
  BOOST_CHECK(contains(set, 0));
  BOOST_CHECK(!contains(set, 1));
  BOOST_CHECK(contains(set, 2));
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(contains), 0, 1);
}
