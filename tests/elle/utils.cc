#include <elle/test.hh>

#include <elle/utils.hh>

namespace
{
  void
  unconst()
  {
    auto i = 42;
    auto const* p = &i;
    *elle::unconst(p) = 21;
    BOOST_TEST(i == 21);
  }
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();

  suite.add(BOOST_TEST_CASE(unconst));
}
