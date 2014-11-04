#include <unordered_map>

#include <elle/test.hh>

static
void
contains()
{
  std::unordered_map<int, int> map({{0, 0}, {2, 2}});
  BOOST_CHECK(contains(map, 0));
  BOOST_CHECK(!contains(map, 1));
  BOOST_CHECK(contains(map, 2));
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(contains), 0, 1);
}
