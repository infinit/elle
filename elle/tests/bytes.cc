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

static
void
pretty()
{
#define CHECK(In, Eng, Comp)                                    \
  BOOST_CHECK_EQUAL(elle::human_data_size(In, true), Eng);      \
  BOOST_CHECK_EQUAL(elle::human_data_size(In, false), Comp)

  CHECK(1000_B, "1.0 kB", "1000.0 B");
  CHECK(1024_B,   "1.0 kB", "1.0 kiB");

  CHECK(1_gB,    "1.0 GB", "953.7 MiB");
  CHECK(1_giB,   "1.1 GB",   "1.0 GiB");

#undef CHECK
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(bytes), 0, 1);
  master.add(BOOST_TEST_CASE(pretty), 0, 1);
}
