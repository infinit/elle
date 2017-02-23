#include <elle/test.hh>

#include <elle/Defaulted.hh>

static
int
f(boost::optional<int> env, elle::Defaulted<int> x = elle::defaulted(42))
{
  if (x || !env)
    return x.get();
  else
    return env.get();
}

static
void
basics()
{
  BOOST_CHECK_EQUAL(f({}), 42);
  BOOST_CHECK_EQUAL(f({}, 51), 51);
  BOOST_CHECK_EQUAL(f(64), 64);
  BOOST_CHECK_EQUAL(f(64, 86), 86);
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(basics));
}
