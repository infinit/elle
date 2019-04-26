#include <array>

#include <elle/print.hh>
#include <elle/test.hh>

static
void
print()
{
  BOOST_TEST(elle::print("{}", std::array<int, 0>()) == "[]");
  BOOST_TEST(elle::print("{}", std::array<int, 3>{0, 1, 2}) == "[0, 1, 2]");
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(print));
}
