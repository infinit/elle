#include <elle/flat-set.hh>
#include <elle/printf.hh>
#include <elle/test.hh>

namespace
{
  using Ints = boost::container::flat_set<int>;
  
  void
  check_printf()
  {
    auto ints = Ints{1, 2, 3};
    BOOST_TEST(elle::sprintf("%x", ints) == "{1, 2, 3}");
  }
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(check_printf));
}
