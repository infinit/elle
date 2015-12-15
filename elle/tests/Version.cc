#include <elle/Version.hh>
#include <elle/test.hh>
#include <elle/serialization/Error.hh>

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

static
std::size_t
h(elle::Version const& v)
{
  return std::hash<elle::Version>()(v);
}

static
void
hash()
{
  BOOST_CHECK_EQUAL(h({1, 2, 3}), h({1, 2, 3}));
  BOOST_CHECK_NE(h({1, 2, 3}), h({3, 2, 1}));
}

static
void
from_string()
{
  // Working usage cases.
  elle::Version major = elle::Version::from_string("1");
  elle::Version major_minor = elle::Version::from_string("1.2");
  elle::Version full = elle::Version::from_string("1.2.3");

  BOOST_CHECK_EQUAL(major, elle::Version(1, 0, 0));
  BOOST_CHECK_EQUAL(major_minor, elle::Version(1, 2, 0));
  BOOST_CHECK_EQUAL(full, elle::Version(1, 2, 3));

  // Failing cases.
  BOOST_CHECK_THROW(elle::Version::from_string("nicolas"),
                    elle::serialization::Error);

  BOOST_CHECK_THROW(elle::Version::from_string(""),
                    elle::serialization::Error);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();

  // Default value
  suite.add(BOOST_TEST_CASE(std::bind(test_default)));

  // == tests
  suite.add(BOOST_TEST_CASE(std::bind(test_equal, 1, 2, 3, 1, 2, 3, true)));
  suite.add(BOOST_TEST_CASE(std::bind(test_equal, 1, 2, 3, 2, 2, 3, false)));
  suite.add(BOOST_TEST_CASE(std::bind(test_equal, 1, 2, 3, 1, 3, 3, false)));
  suite.add(BOOST_TEST_CASE(std::bind(test_equal, 1, 2, 3, 1, 2, 4, false)));
  suite.add(BOOST_TEST_CASE(std::bind(test_equal, 1, 2, 3, 4, 5, 6, false)));

  // != tests
  suite.add(BOOST_TEST_CASE(std::bind(test_not_equal, 1, 2, 3, 3, 2, 3, true)));
  suite.add(BOOST_TEST_CASE(std::bind(test_not_equal, 1, 2, 3, 1, 3, 3, true)));
  suite.add(BOOST_TEST_CASE(std::bind(test_not_equal, 3, 2, 1, 3, 2, 1, false)));

  // < tests
  suite.add(BOOST_TEST_CASE(std::bind(test_less, 1, 2, 3, 2, 2, 3, true)));
  suite.add(BOOST_TEST_CASE(std::bind(test_less, 1, 2, 3, 1, 3, 3, true)));
  suite.add(BOOST_TEST_CASE(std::bind(test_less, 1, 2, 3, 1, 2, 4, true)));
  suite.add(BOOST_TEST_CASE(std::bind(test_less, 1, 2, 3, 1, 2, 3, false)));
  suite.add(BOOST_TEST_CASE(std::bind(test_less, 1, 2, 3, 0, 2, 3, false)));
  suite.add(BOOST_TEST_CASE(std::bind(test_less, 1, 2, 3, 1, 1, 3, false)));
  suite.add(BOOST_TEST_CASE(std::bind(test_less, 1, 2, 3, 1, 2, 2, false)));
  suite.add(BOOST_TEST_CASE(std::bind(test_less, 1, 2, 3, 1, 1, 4, false)));

  // <= tests (same as less than, except for equal case)
  suite.add(BOOST_TEST_CASE(std::bind(test_less_equal, 1, 2, 3, 2, 2, 3, true)));
  suite.add(BOOST_TEST_CASE(std::bind(test_less_equal, 1, 2, 3, 1, 3, 3, true)));
  suite.add(BOOST_TEST_CASE(std::bind(test_less_equal, 1, 2, 3, 1, 2, 4, true)));
  suite.add(BOOST_TEST_CASE(std::bind(test_less_equal, 1, 2, 3, 1, 2, 3, true)));
  suite.add(BOOST_TEST_CASE(std::bind(test_less_equal, 1, 2, 3, 0, 2, 3, false)));
  suite.add(BOOST_TEST_CASE(std::bind(test_less_equal, 1, 2, 3, 1, 1, 3, false)));
  suite.add(BOOST_TEST_CASE(std::bind(test_less_equal, 1, 2, 3, 1, 2, 2, false)));
  suite.add(BOOST_TEST_CASE(std::bind(test_less_equal, 1, 2, 3, 1, 1, 4, false)));

  // > tests (opposite of <, except for equal case)
  suite.add(BOOST_TEST_CASE(std::bind(test_greater, 1, 2, 3, 2, 2, 3, false)));
  suite.add(BOOST_TEST_CASE(std::bind(test_greater, 1, 2, 3, 1, 3, 3, false)));
  suite.add(BOOST_TEST_CASE(std::bind(test_greater, 1, 2, 3, 1, 2, 4, false)));
  suite.add(BOOST_TEST_CASE(std::bind(test_greater, 1, 2, 3, 1, 2, 3, false)));
  suite.add(BOOST_TEST_CASE(std::bind(test_greater, 1, 2, 3, 0, 2, 3, true)));
  suite.add(BOOST_TEST_CASE(std::bind(test_greater, 1, 2, 3, 1, 1, 3, true)));
  suite.add(BOOST_TEST_CASE(std::bind(test_greater, 1, 2, 3, 1, 2, 2, true)));
  suite.add(BOOST_TEST_CASE(std::bind(test_greater, 1, 2, 3, 1, 1, 4, true)));

  // >= tests (same results as >, except for equal case)
  suite.add(BOOST_TEST_CASE(std::bind(test_greater_equal, 1, 2, 3, 2, 2, 3, false)));
  suite.add(BOOST_TEST_CASE(std::bind(test_greater_equal, 1, 2, 3, 1, 3, 3, false)));
  suite.add(BOOST_TEST_CASE(std::bind(test_greater_equal, 1, 2, 3, 1, 2, 4, false)));
  suite.add(BOOST_TEST_CASE(std::bind(test_greater_equal, 1, 2, 3, 1, 2, 3, true)));
  suite.add(BOOST_TEST_CASE(std::bind(test_greater_equal, 1, 2, 3, 0, 2, 3, true)));
  suite.add(BOOST_TEST_CASE(std::bind(test_greater_equal, 1, 2, 3, 1, 1, 3, true)));
  suite.add(BOOST_TEST_CASE(std::bind(test_greater_equal, 1, 2, 3, 1, 2, 2, true)));
  suite.add(BOOST_TEST_CASE(std::bind(test_greater_equal, 1, 2, 3, 1, 1, 4, true)));

  // hashing
  suite.add(BOOST_TEST_CASE(hash));
  suite.add(BOOST_TEST_CASE(from_string));
}
