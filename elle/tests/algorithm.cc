# include <algorithm>

# include <elle/test.hh>

static
void
all()
{
  std::vector<int> v{1, 3, 5};
  BOOST_CHECK(!all(v, [] (int i) { return i == 3; }));
  BOOST_CHECK(all(v, [] (int i) { return i > 0; }));
  BOOST_CHECK(!all(v, [] (int i) { return i % 2 == 0; }));
}

static
void
any()
{
  std::vector<int> v{1, 3, 5};
  BOOST_CHECK(any(v, [] (int i) { return i == 3; }));
  BOOST_CHECK(any(v, [] (int i) { return i > 0; }));
  BOOST_CHECK(!any(v, [] (int i) { return i % 2 == 0; }));
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(all));
  master.add(BOOST_TEST_CASE(any));
}
