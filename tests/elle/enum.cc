#include <elle/enum.hh>

#include <elle/test.hh>

ELLE_ENUM(
  Test,
  (A, 1),
  (b, 3),
  (C, 5));

static
void
basics()
{
  BOOST_CHECK(elle::print("{} {} {}", Test::A, Test::b, Test::C) == "A b C");
  BOOST_CHECK(int(Test::A) == 1);
  BOOST_CHECK(int(Test::b) == 3);
  BOOST_CHECK(int(Test::C) == 5);
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(basics));
}
