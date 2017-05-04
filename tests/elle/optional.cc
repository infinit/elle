#include <elle/optional.hh>
#include <elle/test.hh>

#include <boost/unordered_set.hpp>

static
void
print()
{
  boost::optional<bool> o;
  BOOST_CHECK(elle::sprintf("%s", o) == "null");
  o = true;
  BOOST_CHECK(elle::sprintf("%s", o) == "true");
}

template <template <typename ...> class C>
static
void
hash()
{
  C<boost::optional<int>> s;
  BOOST_CHECK(s.size() == 0u);
  s.emplace(boost::none);
  BOOST_CHECK(s.size() == 1u);
  s.emplace(boost::none);
  BOOST_CHECK(s.size() == 1u);
  s.emplace(24);
  BOOST_CHECK(s.size() == 2u);
  s.emplace(24);
  BOOST_CHECK(s.size() == 2u);
  s.emplace(28);
  BOOST_CHECK(s.size() == 3u);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(print));
  suite.add(ELLE_TEST_CASE(&hash<boost::unordered_set>, "hash_boost"));
  suite.add(ELLE_TEST_CASE(&hash<std::unordered_set>, "hash_std"));
}
