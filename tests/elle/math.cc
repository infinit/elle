#include <elle/math.hh>

#include <string>

#include <boost/range/irange.hpp>

#include <elle/range.hh>
#include <elle/test.hh>

static
void
sum()
{
  BOOST_TEST(elle::sum() == 0);
  BOOST_TEST(elle::sum(1) == 1);
  BOOST_TEST(elle::sum(1, 2) == 3);
  BOOST_TEST(elle::sum(1, 2, 3) == 6);
}

static
void
product()
{
  BOOST_TEST(elle::product() == 1);
  BOOST_TEST(elle::product(1) == 1);
  BOOST_TEST(elle::product(1, 2) == 2);
  BOOST_TEST(elle::product(1, 2, 3) == 6);
  BOOST_TEST(elle::product(0, 1, 2, 3) == 0);
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(sum));
  master.add(BOOST_TEST_CASE(product));
}
