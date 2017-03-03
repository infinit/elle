#include <elle/das/Symbol.hh>
#include <elle/das/tuple.hh>
#include <elle/test.hh>

ELLE_DAS_SYMBOL(foo);
ELLE_DAS_SYMBOL(bar);

static
void
basics()
{
  auto t = elle::das::make_tuple(foo = 3, bar = "lol");
  BOOST_CHECK_EQUAL(t.foo, 3);
  BOOST_CHECK_EQUAL(t.bar, "lol");
}

/*-------.
| Driver |
`-------*/

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(basics));
}
