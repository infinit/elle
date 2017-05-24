/// Check <elle/algorithm.hh>.

#include <elle/test.hh>
#include <elle/algorithm.hh>

namespace
{
  void
  push_back()
  {
    {
      using MoveOnly = std::unique_ptr<int>;
      using MOs = std::vector<MoveOnly>;

      auto v1 = MOs{};
      v1.emplace_back();
      v1.emplace_back();

      auto v2 = MOs{};
      v2.emplace_back();
      v2.emplace_back();
      v2.emplace_back();

      elle::push_back(v1, std::move(v2));
      BOOST_TEST(v1.size() == 5);
    }
    {
      using Ints = std::vector<int>;
      auto v = Ints{1, 2, 3};
      auto i = elle::push_back(v, 4, 5, 6);
      BOOST_TEST(*i == 4);
      BOOST_TEST(v == (Ints{1, 2, 3, 4, 5, 6}));
      elle::push_back(v, 7);
      BOOST_TEST(v == (Ints{1, 2, 3, 4, 5, 6, 7}));
    }
  }
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(push_back), 0, 1);
}
