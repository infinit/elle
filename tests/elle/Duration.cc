#include <iostream>
#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <elle/Duration.hh>
#include <elle/compiler.hh>
#include <elle/date/date.h>
#include <elle/test.hh>

using namespace std::literals;

namespace
{
  elle::Duration id(elle::Duration d = 30s)
  {
    return d;
  }

  elle::Duration optid(elle::DurationOpt d = 30s)
  {
    return *d;
  }

  void
  optional()
  {
    auto s = elle::Duration{1s};
    BOOST_TEST(s == 1000ms);

    BOOST_TEST(elle::Duration{1ms} == 1ms);
    BOOST_TEST(*elle::DurationOpt{1ms} == 1ms);

    BOOST_TEST(elle::Duration{30s} == 30s);
    BOOST_TEST(*elle::DurationOpt{30s} == 30s);
    BOOST_TEST(id() == 30s);
    BOOST_TEST(optid() == 30s);
  }

  void
  io()
  {
    auto const ref = std::string{"20131009T233600"};
    // Check Boost's format.  We use it to forge dates for S3.
    {
      auto const time
        = boost::posix_time::ptime
        (boost::gregorian::date(2013, boost::gregorian::Oct, 9),
         boost::posix_time::hours(23) + boost::posix_time::minutes(36));
      BOOST_TEST(to_iso_string(time) == ref);
    }
    // Likewise, but using Hinnant's Date.
    {
      using namespace date;
      auto const time = sys_days{2013_y/oct/9} + 23h + 36min;
      BOOST_TEST(format("%Y%m%dT%H%M%S", time) == ref);
      BOOST_TEST(format("%Y%m%d", time) == ref.substr(0, 8));
    }
    // Likewise, using the date parser.
#if ! GCC_VERSION_LTE(4, 9)
    {
      using namespace date;
      auto&& s = std::istringstream{ref};
      auto time = elle::Time{};
      from_stream(s, "%Y%m%dT%H%M%S", time);
      auto const ref = sys_days{2013_y/oct/9} + 23h + 36min;
      BOOST_CHECK_EQUAL(time, ref);
    }
#endif
  }
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(optional), 0, 1);
  master.add(BOOST_TEST_CASE(io), 0, 1);
}
