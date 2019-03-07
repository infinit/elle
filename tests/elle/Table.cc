#include <elle/Table.hh>

#include <string>

#include <boost/range/irange.hpp>

#include <elle/range.hh>
#include <elle/test.hh>

static
void
pod()
{
  auto constexpr w = 3;
  auto constexpr h = 2;
  elle::Table<int, 2> table(w, h);
  for (auto i : boost::irange(0, w))
    for (auto j : boost::irange(0, h))
      BOOST_TEST((table.at(i, j) = w * i + j) == w * i + j);
  for (auto i : boost::irange(0, w))
    for (auto j : boost::irange(0, h))
      BOOST_TEST(table.at(i, j) == w * i + j);
}

static
void
object()
{
  auto constexpr w = 3;
  auto constexpr h = 2;
  elle::Table<std::string, 2> table(w, h);
  for (auto i : boost::irange(0, w))
    for (auto j : boost::irange(0, h))
      BOOST_TEST(table.at(i, j) == "");
  for (auto i : boost::irange(0, w))
    for (auto j : boost::irange(0, h))
      BOOST_TEST((table.at(i, j) = std::to_string(w * i + j)) ==
                 std::to_string(w * i + j));
  for (auto i : boost::irange(0, w))
    for (auto j : boost::irange(0, h))
      BOOST_TEST(table.at(i, j) == std::to_string(w * i + j));
}

static
void
bounds()
{
  auto constexpr w = 3;
  auto constexpr h = 2;
  elle::Table<int, 2> table(w, h);
  for (auto i : boost::irange(0, w))
    BOOST_CHECK_THROW(table.at(i, h), elle::Error);
  for (auto j : boost::irange(0, h))
    BOOST_CHECK_THROW(table.at(w, j), elle::Error);
  BOOST_CHECK_THROW(table.at(-1, 0), elle::Error);
  BOOST_CHECK_THROW(table.at(0, -1), elle::Error);
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(pod));
  master.add(BOOST_TEST_CASE(object));
  master.add(BOOST_TEST_CASE(bounds));
}
