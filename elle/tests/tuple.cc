#include <tuple>

#include <elle/test.hh>

static
void
print()
{
  BOOST_CHECK_EQUAL(elle::sprintf("%s", std::make_tuple()), "()");
  BOOST_CHECK_EQUAL(elle::sprintf("%s", std::make_tuple(0, "foo", true)),
                    "(0, foo, true)");
}

struct Nope
{
  Nope() = default;
  Nope(Nope const&) = delete;
  Nope(Nope&&) = default;
};

static
void
forward()
{
  BOOST_CHECK_EQUAL(
    forward_tuple([] (int a, int b) { return a + b; }, std::make_tuple(20, 22)),
    42);
  forward_tuple([] (Nope n) { return n; }, std::make_tuple(Nope()));
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(print), 0, 1);
  master.add(BOOST_TEST_CASE(forward), 0, 1);
}
