#include <elle/IntRange.hh>
#include <elle/printf.hh>
#include <elle/test.hh>

namespace int_range
{
  static
  void
  basics()
  {
    auto range0to99 = elle::IntRange{0, 100};
    BOOST_CHECK_EQUAL(elle::sprintf("%s", range0to99),
                      std::string{"(0, 100)"});
    BOOST_CHECK_EQUAL(range0to99.size(), 100);
    BOOST_CHECK(!range0to99.empty());

    auto range50to149 = elle::IntRange{50, 100};
    BOOST_CHECK_EQUAL(elle::sprintf("%s", range50to149),
                      std::string{"(50, 100)"});
    auto range10to98 = elle::IntRange{10, 89};
    auto range1000to9999 = elle::IntRange{1000, 9900};

    BOOST_CHECK(range0to99.contains(range10to98));
    BOOST_CHECK(range0to99.overlaps(range50to149));
    BOOST_CHECK(range50to149.overlaps(range0to99));
    BOOST_CHECK_GT(range1000to9999, range0to99);
    BOOST_CHECK_LT(range0to99, range1000to9999);
  }

  static
  void
  arithmetic()
  {
    auto range0to99 = elle::IntRange{0, 100};
    auto range50to149 = elle::IntRange{50, 100};
    auto range10to98 = elle::IntRange{10, 89};
    auto range1000to9999 = elle::IntRange{1000, 9900};

    /// Prefix.
    auto range50to149_minus_range0to99 = range50to149 - range0to99;
    auto ranges_100to149 = elle::IntRanges{{100, 50}};
    BOOST_CHECK_EQUAL(range50to149_minus_range0to99, ranges_100to149);
    /// Suffix.
    auto range0to99_minus_range50to149 = range0to99 - range50to149;
    auto ranges_0to49 = elle::IntRanges{{0, 50}};
    BOOST_CHECK_EQUAL(range0to99_minus_range50to149, ranges_0to49);
    /// Disjoint.
    auto range1000to9999_minus_range0to99 = range1000to9999 - range0to99;
    BOOST_CHECK_EQUAL(range1000to9999_minus_range0to99, range1000to9999);
    auto range0to99_minus_range1000to9999 = range0to99 - range1000to9999;
    BOOST_CHECK_EQUAL(range0to99_minus_range1000to9999, range0to99);
    /// Contained.
    auto range0to99_minus_range10to98 = range0to99 - range10to98;
    auto ranges_0to9_99to100 = elle::IntRanges{{
        elle::IntRange{0, 10},
        elle::IntRange{99, 1}
      }};
    BOOST_CHECK_EQUAL(range0to99_minus_range10to98, ranges_0to9_99to100);
    /// Fully contained.
    auto range10to98_minus_range0to99 = range10to98 - range0to99;
    BOOST_CHECK_EQUAL(range10to98_minus_range0to99,
                      elle::IntRanges{});
  }
}

namespace int_ranges
{
  static
  void
  basics()
  {
    auto ranges_0to1_4to5 = elle::IntRanges{{
      elle::IntRange{0, 2},
      elle::IntRange{4, 2}
    }};
    BOOST_CHECK_EQUAL(elle::sprintf("%s", ranges_0to1_4to5),
                      std::string{"{(0, 2), (4, 2)}"});
    BOOST_CHECK(ranges_0to1_4to5.contains(elle::IntRange{1, 1}));
    BOOST_CHECK(!ranges_0to1_4to5.contains(elle::IntRange{2, 8}));
    BOOST_CHECK(ranges_0to1_4to5.contains(1));
    BOOST_CHECK(!ranges_0to1_4to5.contains(10));
    auto res = std::vector<elle::IntRange>{};
    for (auto i: ranges_0to1_4to5)
      res.push_back(i);
    std::sort(res.begin(), res.end());
    auto expected_res = std::vector<elle::IntRange>{{0, 2}, {4, 2}};
    BOOST_CHECK_EQUAL(res, expected_res);
    BOOST_CHECK_EQUAL(ranges_0to1_4to5.next_free(0), 2);
    BOOST_CHECK_EQUAL(ranges_0to1_4to5.next_free(4), 6);

    ranges_0to1_4to5 += elle::IntRange{6, 2};
    BOOST_CHECK_EQUAL(elle::sprintf("%s", ranges_0to1_4to5),
                      std::string{"{(0, 2), (4, 4)}"});
  }
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(int_range::basics));
  suite.add(BOOST_TEST_CASE(int_range::arithmetic));
  suite.add(BOOST_TEST_CASE(int_ranges::basics));
}
