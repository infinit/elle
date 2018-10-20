#include <elle/Exit.hh>

#include <elle/test.hh>

static
void
basics()
{
  try
  {
    throw elle::Exit();
  }
  catch (elle::Exit const& e)
  {
    BOOST_CHECK(e.return_code() == 0);
  }
  try
  {
    throw elle::Exit(42);
  }
  catch (elle::Exit const& e)
  {
    BOOST_CHECK(e.return_code() == 42);
  }
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(basics));
}
