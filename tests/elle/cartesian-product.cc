#include <elle/log.hh>

ELLE_LOG_COMPONENT("coin");

#include <elle/cartesian-product.hh>

#include <boost/range/combine.hpp>

#include <elle/test.hh>

template <typename T>
std::initializer_list<T>
list(std::initializer_list<T>&& l)
{
  return std::forward<decltype(l)>(l);
}

static
void
empty()
{
  auto const empty = std::initializer_list<int>();
  for (auto e: elle::cartesian_product(empty, empty))
    BOOST_ERROR(elle::print("unexpected element: {}", e));
  for (auto e: elle::cartesian_product(list({0, 1, 2}), empty))
    BOOST_ERROR(elle::print("unexpected element: {}", e));
  for (auto e: elle::cartesian_product(empty, list({0, 1, 2})))
    BOOST_ERROR(elle::print("unexpected element: {}", e));
}

static
void
basics()
{
  auto const product =
    elle::cartesian_product(list({0, 1, 2}), list({3, 4, 5}));
  auto const expected = list<std::pair<int, int>>({
      {0, 3}, {0, 4}, {0, 5},
      {1, 3}, {1, 4}, {1, 5},
      {2, 3}, {2, 4}, {2, 5},
    });
  for (auto const e: boost::combine(product, expected))
    BOOST_CHECK_EQUAL(e.get<0>(), e.get<1>());
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();

  suite.add(BOOST_TEST_CASE(empty));
  suite.add(BOOST_TEST_CASE(basics));
}
