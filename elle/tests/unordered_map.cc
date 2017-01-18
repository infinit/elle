#include <unordered_map>
#include <unordered_set>

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

static
void
keys_iterator()
{
  std::unordered_map<int, int> map({{0, 10}, {1, 11}, {2, 12}});
  std::unordered_set<int> keys({0, 1, 2});
  for (auto it = iter_keys(map); it != map.end(); ++it)
    BOOST_CHECK(keys.erase(*it));
  BOOST_CHECK(keys.empty());
}

static
void
values_iterator()
{
  auto map = std::unordered_map<int, int>({{0, 10}, {1, 11}, {2, 12}});
  auto values = std::unordered_set<int>({10, 11, 12});
  // Check values are iterated by reference.
  auto it = iter_values(map);
  BOOST_CHECK_EQUAL(&*it, &map.at(*it - 10));
  for (auto it = iter_values(map); it != map.end(); ++it)
    BOOST_CHECK(values.erase(*it));
  BOOST_CHECK(values.empty());
}

#include <elle/unordered_map.hh>

static
void
missing_key_exception()
{
  elle::unordered_map<int, int> map({{0, 0}, {1, 1}});
  BOOST_CHECK_EQUAL(map.at(0), 0);
  BOOST_CHECK_EQUAL(map.at(1), 1);
  BOOST_CHECK_THROW(map.at(2), elle::Error);
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(contains), 0, 1);
  master.add(BOOST_TEST_CASE(keys_iterator), 0, 1);
  master.add(BOOST_TEST_CASE(values_iterator), 0, 1);
  master.add(BOOST_TEST_CASE(missing_key_exception), 0, 1);
}
