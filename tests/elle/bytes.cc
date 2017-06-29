#include <elle/bytes.hh>


#include <elle/test.hh>

namespace
{
  void
  bytes()
  {
    BOOST_TEST(1000_B   == 1_kB);
    BOOST_TEST(1000_kB  == 1_MB);
    BOOST_TEST(1000_MB  == 1_GB);
    BOOST_TEST(1024_B   == 1_KiB);
    BOOST_TEST(1024_KiB == 1_MiB);
    BOOST_TEST(1024_MiB == 1_GiB);
  }

  static
  void
  pretty()
  {
#define CHECK(In, Eng, Comp)                                      \
    BOOST_CHECK_EQUAL(elle::human_data_size(In, true), Eng);      \
    BOOST_CHECK_EQUAL(elle::human_data_size(In, false), Comp)

    CHECK(1000_B, "1.0 kB", "1000.0 B");
    CHECK(1024_B,   "1.0 kB", "1.0 KiB");

    CHECK(1_GB,    "1.0 GB", "953.7 MiB");
    CHECK(1_GiB,   "1.1 GB",   "1.0 GiB");

#undef CHECK
  }
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(bytes), 0, 1);
  master.add(BOOST_TEST_CASE(pretty), 0, 1);
}
