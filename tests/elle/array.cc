#include <array>

#include <elle/print.hh>
#include <elle/test.hh>

namespace operators
{
  static
  void
  scalar()
  {
    using A = std::array<int, 3>;
    BOOST_TEST((A{0, 1, 2}) + 1 == (A{1, 2, 3}));
    BOOST_TEST((A{0, 1, 2}) - 1 == (A{-1, 0, 1}));
    BOOST_TEST((A{0, 1, 2}) * 2 == (A{0, 2, 4}));
    BOOST_TEST((A{0, 2, 4}) / 2 == (A{0, 1, 2}));
    BOOST_TEST((A{0, 5, 8}) % 2 == (A{0, 1, 0}));
  }

  static
  void
  array()
  {
    using A = std::array<int, 3>;
    BOOST_TEST((A{0, 1, 2} + A{3, 4, 5}) == (A{3, 5, 7}));
    BOOST_TEST((A{0, 1, 2} - A{5, 4, 3}) == (A{-5, -3, -1}));
    BOOST_TEST((A{0, 1, 2} * A{2, 3, 4}) == (A{0, 3, 8}));
    BOOST_TEST((A{4, 8, 16} / A{2, 1, 4}) == (A{2, 8, 4}));
    BOOST_TEST((A{4, 8, 16} % A{2, 3, 5}) == (A{0, 2, 1}));
  }
}

static
void
compare()
{
  auto a = std::array<int, 3>{0, 1, 2};
  BOOST_TEST(a == std::tuple(0, 1, 2));
  BOOST_TEST(!(a == std::tuple(0, 1, 3)));
}

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
  suite.add(BOOST_TEST_CASE(compare));
  {
    boost::unit_test::test_suite* op = BOOST_TEST_SUITE("operators");
    suite.add(op);
    using namespace operators;
    op->add(BOOST_TEST_CASE(scalar));
    op->add(BOOST_TEST_CASE(array));
  }
  suite.add(BOOST_TEST_CASE(print));
}
