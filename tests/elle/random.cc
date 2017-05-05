#include <list>
#include <vector>

#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/range/algorithm_ext/is_sorted.hpp>

#include <elle/log.hh>
#include <elle/random.hh>
#include <elle/test.hh>

ELLE_LOG_COMPONENT("elle.test.random");

namespace
{
  using Histo = std::vector<int>;

  /*-----------.
  | pick_one.  |
  `-----------*/

  /// Check that `histo` contains only 100 odd numbers.
  void
  check_histo_pick_one(Histo const& histo)
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
    auto histo = Histo(10, 0);
    for (int i = 0; i < 100; ++i)
    {
      auto num = *elle::pick_one(range);
      BOOST_TEST(0 <= num);
      BOOST_TEST(num <= 9);
      ++histo[num];
    }
    check_histo_pick_one(histo);
  }

  /// Pick one with filter covers the whole range.
  void
  pick_one_filtered()
  {
    auto range = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto histo = Histo(10, 0);
    for (int i = 0; i < 100; ++i)
    {
      auto num = *elle::pick_one(range, [](auto i) { return i % 2; });
      BOOST_TEST(0 <= num);
      BOOST_TEST(num <= 9);
      ++histo[num];
    }
    check_histo_pick_one(histo);
  }


  /*---------.
  | pick_n.  |
  `---------*/

  void
  check_histo_pick_n(Histo const& histo)
  {
    ELLE_LOG("histo: %s", histo);
    int sum = 0;
    for (auto i = 0u; i < histo.size(); ++i)
    {
      BOOST_TEST(250 <= histo[i]);
      BOOST_TEST(histo[i] <= 350);
      sum += histo[i];
    }
    BOOST_TEST(sum == 3000);
  }

  /// Check the selection of integers.
  void
  pick_n_integers()
  {
    auto histo = Histo(10, 0);
    for (int i = 0; i < 1000; ++i)
    {
      auto nums = elle::pick_n(3, 10);
      ELLE_LOG("nums = %s", nums);
      BOOST_TEST(boost::algorithm::is_strictly_increasing(nums));
      for (auto n: nums)
        ++histo[n];
    }
    check_histo_pick_n(histo);
  }

  /// Check the selection of iterators in a container.
  template <typename Range>
  void
  pick_n_iterators_impl(Range const& r)
  {
    for (auto i: r)
    {
      BOOST_TEST_REQUIRE(0 <= i);
      BOOST_TEST_REQUIRE(i < 10);
    }
    auto histo = Histo(10, 0);
    for (int i = 0; i < 1000; ++i)
      for (auto i: elle::pick_n(3, r))
        ++histo[*i];
    check_histo_pick_n(histo);
  }

  void
  pick_n_iterators()
  {
    pick_n_iterators_impl(std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    // Without random access.
    pick_n_iterators_impl(std::list<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
  }
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(pick_one));
  suite.add(BOOST_TEST_CASE(pick_one_filtered));
  suite.add(BOOST_TEST_CASE(pick_n_integers));
  suite.add(BOOST_TEST_CASE(pick_n_iterators));
}
