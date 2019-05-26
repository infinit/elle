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

  void
  tail()
  {
    BOOST_TEST(elle::tail(std::string{"foobar"}, "foo") == std::string{"bar"});
    BOOST_TEST(!elle::tail(std::string{"foobar"}, "bar"));
  }

  void
  more_or_less()
  {
#define ELLE_ALGORITHM_TEST(F, Y, N)            \
    BOOST_TEST(! F(0, 0));                      \
    BOOST_TEST(Y F(1, 0));                      \
    BOOST_TEST(N F(0, 1));                      \
    BOOST_TEST(! F(0u, 0));                     \
    BOOST_TEST(Y F(1u, 0));                     \
    BOOST_TEST(N F(0u, 1));                     \
    BOOST_TEST(! F(0, 0u));                     \
    BOOST_TEST(Y F(1, 0u));                     \
    BOOST_TEST(N F(0, 1u));                     \
    BOOST_TEST(! F(0u, 0u));                    \
    BOOST_TEST(Y F(1u, 0u));                    \
    BOOST_TEST(N F(0u, 1u));                    \
    BOOST_TEST(Y F(0u, -1));                    \
    BOOST_TEST(N F(-1, 0u));

    ELLE_ALGORITHM_TEST(elle::less, !, );
    ELLE_ALGORITHM_TEST(elle::greater, , !);
#undef ELLE_ALGORITHM_TEST
  }
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(push_back), 0, 1);
  master.add(BOOST_TEST_CASE(tail), 0, 1);
  master.add(BOOST_TEST_CASE(more_or_less), 0, 1);
}
