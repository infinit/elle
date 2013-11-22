#include <elle/Version.hh>
#include <elle/test.hh>

static
void
test_default()
{
  elle::Version def;
  elle::Version expected(0, 0, 0);
  BOOST_CHECK_EQUAL(def, expected);
}

static
void
test_equal(int maj1, int min1, int submin1,
           int maj2, int min2, int submin2,
           bool result)
{
  elle::Version first(maj1, min1, submin1);
  elle::Version second(maj2, min2, submin2);
  BOOST_CHECK_EQUAL((first == second), result);
}

static
void
test_not_equal(int maj1, int min1, int submin1,
               int maj2, int min2, int submin2,
               bool result)
{
  elle::Version first(maj1, min1, submin1);
  elle::Version second(maj2, min2, submin2);
  BOOST_CHECK_EQUAL((first != second), result);
}

static
void
test_less(int maj1, int min1, int submin1,
          int maj2, int min2, int submin2,
          bool result)
{
  elle::Version first(maj1, min1, submin1);
  elle::Version second(maj2, min2, submin2);
  BOOST_CHECK_EQUAL((first < second), result);
}

static
void
test_less_equal(int maj1, int min1, int submin1,
                int maj2, int min2, int submin2,
                bool result)
{
  elle::Version first(maj1, min1, submin1);
  elle::Version second(maj2, min2, submin2);
  BOOST_CHECK_EQUAL((first <= second), result);
}

static
void
test_greater(int maj1, int min1, int submin1,
             int maj2, int min2, int submin2,
             bool result)
{
  elle::Version first(maj1, min1, submin1);
  elle::Version second(maj2, min2, submin2);
  BOOST_CHECK_EQUAL((first > second), result);
}

static
void
test_greater_equal(int maj1, int min1, int submin1,
                   int maj2, int min2, int submin2,
                   bool result)
{
  elle::Version first(maj1, min1, submin1);
  elle::Version second(maj2, min2, submin2);
  BOOST_CHECK_EQUAL((first >= second), result);
}

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* basics = BOOST_TEST_SUITE("Basics");
  boost::unit_test::framework::master_test_suite().add(basics);

  // Default value
  basics->add(BOOST_TEST_CASE(std::bind(test_default)));

  // == tests
  basics->add(BOOST_TEST_CASE(std::bind(test_equal, 1, 2, 3, 1, 2, 3, true)));
  basics->add(BOOST_TEST_CASE(std::bind(test_equal, 1, 2, 3, 2, 2, 3, false)));
  basics->add(BOOST_TEST_CASE(std::bind(test_equal, 1, 2, 3, 1, 3, 3, false)));
  basics->add(BOOST_TEST_CASE(std::bind(test_equal, 1, 2, 3, 1, 2, 4, false)));
  basics->add(BOOST_TEST_CASE(std::bind(test_equal, 1, 2, 3, 4, 5, 6, false)));

  // != tests
  basics->add(BOOST_TEST_CASE(std::bind(test_not_equal, 1, 2, 3, 3, 2, 3, true)));
  basics->add(BOOST_TEST_CASE(std::bind(test_not_equal, 1, 2, 3, 1, 3, 3, true)));
  basics->add(BOOST_TEST_CASE(std::bind(test_not_equal, 3, 2, 1, 3, 2, 1, false)));

  // < tests
  basics->add(BOOST_TEST_CASE(std::bind(test_less, 1, 2, 3, 2, 2, 3, true)));
  basics->add(BOOST_TEST_CASE(std::bind(test_less, 1, 2, 3, 1, 3, 3, true)));
  basics->add(BOOST_TEST_CASE(std::bind(test_less, 1, 2, 3, 1, 2, 4, true)));
  basics->add(BOOST_TEST_CASE(std::bind(test_less, 1, 2, 3, 1, 2, 3, false)));
  basics->add(BOOST_TEST_CASE(std::bind(test_less, 1, 2, 3, 0, 2, 3, false)));
  basics->add(BOOST_TEST_CASE(std::bind(test_less, 1, 2, 3, 1, 1, 3, false)));
  basics->add(BOOST_TEST_CASE(std::bind(test_less, 1, 2, 3, 1, 2, 2, false)));
  basics->add(BOOST_TEST_CASE(std::bind(test_less, 1, 2, 3, 1, 1, 4, false)));

  // <= tests (same as less than, except for equal case)
  basics->add(BOOST_TEST_CASE(std::bind(test_less_equal, 1, 2, 3, 2, 2, 3, true)));
  basics->add(BOOST_TEST_CASE(std::bind(test_less_equal, 1, 2, 3, 1, 3, 3, true)));
  basics->add(BOOST_TEST_CASE(std::bind(test_less_equal, 1, 2, 3, 1, 2, 4, true)));
  basics->add(BOOST_TEST_CASE(std::bind(test_less_equal, 1, 2, 3, 1, 2, 3, true)));
  basics->add(BOOST_TEST_CASE(std::bind(test_less_equal, 1, 2, 3, 0, 2, 3, false)));
  basics->add(BOOST_TEST_CASE(std::bind(test_less_equal, 1, 2, 3, 1, 1, 3, false)));
  basics->add(BOOST_TEST_CASE(std::bind(test_less_equal, 1, 2, 3, 1, 2, 2, false)));
  basics->add(BOOST_TEST_CASE(std::bind(test_less_equal, 1, 2, 3, 1, 1, 4, false)));

  // > tests (opposite of <, except for equal case)
  basics->add(BOOST_TEST_CASE(std::bind(test_greater, 1, 2, 3, 2, 2, 3, false)));
  basics->add(BOOST_TEST_CASE(std::bind(test_greater, 1, 2, 3, 1, 3, 3, false)));
  basics->add(BOOST_TEST_CASE(std::bind(test_greater, 1, 2, 3, 1, 2, 4, false)));
  basics->add(BOOST_TEST_CASE(std::bind(test_greater, 1, 2, 3, 1, 2, 3, false)));
  basics->add(BOOST_TEST_CASE(std::bind(test_greater, 1, 2, 3, 0, 2, 3, true)));
  basics->add(BOOST_TEST_CASE(std::bind(test_greater, 1, 2, 3, 1, 1, 3, true)));
  basics->add(BOOST_TEST_CASE(std::bind(test_greater, 1, 2, 3, 1, 2, 2, true)));
  basics->add(BOOST_TEST_CASE(std::bind(test_greater, 1, 2, 3, 1, 1, 4, true)));

  // >= tests (same results as >, except for equal case)
  basics->add(BOOST_TEST_CASE(std::bind(test_greater_equal, 1, 2, 3, 2, 2, 3, false)));
  basics->add(BOOST_TEST_CASE(std::bind(test_greater_equal, 1, 2, 3, 1, 3, 3, false)));
  basics->add(BOOST_TEST_CASE(std::bind(test_greater_equal, 1, 2, 3, 1, 2, 4, false)));
  basics->add(BOOST_TEST_CASE(std::bind(test_greater_equal, 1, 2, 3, 1, 2, 3, true)));
  basics->add(BOOST_TEST_CASE(std::bind(test_greater_equal, 1, 2, 3, 0, 2, 3, true)));
  basics->add(BOOST_TEST_CASE(std::bind(test_greater_equal, 1, 2, 3, 1, 1, 3, true)));
  basics->add(BOOST_TEST_CASE(std::bind(test_greater_equal, 1, 2, 3, 1, 2, 2, true)));
  basics->add(BOOST_TEST_CASE(std::bind(test_greater_equal, 1, 2, 3, 1, 1, 4, true)));
}
