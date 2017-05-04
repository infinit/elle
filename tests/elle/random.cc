#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/range/algorithm_ext/is_sorted.hpp>

#include <elle/log.hh>
#include <elle/random.hh>
#include <elle/test.hh>

ELLE_LOG_COMPONENT("elle.test.random");

namespace
{
  /// Check that `histo` contains only 100 odd numbers.
  void
  check_histo(std::vector<int> const& histo)
  {
    ELLE_LOG("histo: %s", histo);
    int sum = 0;
    for (auto i = 0u; i < histo.size(); ++i)
    {
      sum += histo[i];
      if (i % 2)
        BOOST_TEST(histo[i] != 0);
      else
        BOOST_TEST(histo[i] == 0);
    }
    BOOST_TEST(sum == 100);
  }

  /// Pick one covers the whole range.
  void
  pick_one()
  {
    auto range = {1, 3, 5, 7, 9};
    auto histo = std::vector<int>(10, 0);
    for (int i = 0; i < 100; ++i)
    {
      auto num = *elle::pick_one(range);
      BOOST_TEST(0 <= num);
      BOOST_TEST(num <= 9);
      ++histo[num];
    }
    check_histo(histo);
  }

  /// Pick one with filter covers the whole range.
  void
  pick_one_filtered()
  {
    auto range = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto histo = std::vector<int>(10, 0);
    for (int i = 0; i < 100; ++i)
    {
      auto num = *elle::pick_one(range, [](auto i) { return i % 2; });
      BOOST_TEST(0 <= num);
      BOOST_TEST(num <= 9);
      ++histo[num];
    }
    check_histo(histo);
  }

  void
  pick_n()
  {
    auto histo = std::vector<int>(10, 0);
    for (int i = 0; i < 1000; ++i)
    {
      auto nums = elle::pick_n(3, 10);
      ELLE_LOG("nums = %s", nums);
      BOOST_TEST(boost::algorithm::is_strictly_increasing(nums));
      for (auto n: nums)
        ++histo[n];
    }
    ELLE_LOG("histo: %s", histo);
    for (auto i = 0u; i < histo.size(); ++i)
    {
      BOOST_TEST(250 <= histo[i]);
      BOOST_TEST(histo[i] <= 350);
    }
  }
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(pick_one));
  suite.add(BOOST_TEST_CASE(pick_one_filtered));
  suite.add(BOOST_TEST_CASE(pick_n));
}
