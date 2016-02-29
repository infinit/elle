#include <elle/bytes.hh>


#include <elle/test.hh>

static
void
bytes()
{
  BOOST_CHECK_EQUAL(1000_B,   1_kB);
  BOOST_CHECK_EQUAL(1000_kB,  1_mB);
  BOOST_CHECK_EQUAL(1000_mB,  1_gB);
  BOOST_CHECK_EQUAL(1024_B,   1_kiB);
  BOOST_CHECK_EQUAL(1024_kiB, 1_miB);
  BOOST_CHECK_EQUAL(1024_miB, 1_giB);
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(bytes), 0, 1);
}
