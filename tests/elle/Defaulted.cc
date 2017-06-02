#include <elle/test.hh>

#include <elle/Defaulted.hh>

namespace
{
  int
  f(boost::optional<int> env, elle::Defaulted<int> x = elle::defaulted(42))
  {
    if (x || !env)
      return x.get();
    else
      return env.get();
  }

  void
  basics()
  {
    BOOST_TEST(f({}) == 42);
    BOOST_TEST(f({}, 51) == 51);
    BOOST_TEST(f(64) == 64);
    BOOST_TEST(f(64, 86) == 86);

    {
      auto d = elle::defaulted(42);
      BOOST_TEST(elle::print("%s", d) == "42");
      BOOST_TEST(elle::print("%r", d) == "42 (default)");
    }
  }
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(basics));
}
