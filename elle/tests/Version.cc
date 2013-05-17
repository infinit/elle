#define BOOST_TEST_DYN_LINK

#include <elle/Version.hh>

#include <boost/test/unit_test.hpp>

static
void
test_default()
{
  elle::Version def;
  elle::Version expected(0, 0);
  BOOST_CHECK_EQUAL(def, expected);
}

static
void
test_equal(int maj1, int min1, int maj2, int min2, bool result)
{
	elle::Version first(maj1, min1);
	elle::Version second(maj2, min2);
	BOOST_CHECK_EQUAL((first == second), result);
}

static
void
test_not_equal(int maj1, int min1, int maj2, int min2, bool result)
{
	elle::Version first(maj1, min1);
	elle::Version second(maj2, min2);
	BOOST_CHECK_EQUAL((first != second), result);
}

static
void
test_less(int maj1, int min1, int maj2, int min2, bool result)
{
	elle::Version first(maj1, min1);
	elle::Version second(maj2, min2);
	BOOST_CHECK_EQUAL((first < second), result);
}

static
void
test_less_equal(int maj1, int min1, int maj2, int min2, bool result)
{
	elle::Version first(maj1, min1);
	elle::Version second(maj2, min2);
	BOOST_CHECK_EQUAL((first <= second), result);
}

static
void
test_greater(int maj1, int min1, int maj2, int min2, bool result)
{
	elle::Version first(maj1, min1);
	elle::Version second(maj2, min2);
	BOOST_CHECK_EQUAL((first > second), result);
}

static
void
test_greater_equal(int maj1, int min1, int maj2, int min2, bool result)
{
	elle::Version first(maj1, min1);
	elle::Version second(maj2, min2);
	BOOST_CHECK_EQUAL((first >= second), result);
}



static
bool
test_suite()
{
	boost::unit_test::test_suite* basics = BOOST_TEST_SUITE("Basics");
	boost::unit_test::framework::master_test_suite().add(basics);

        // Default value
	basics->add(BOOST_TEST_CASE(std::bind(test_default)));

	// == tests
	basics->add(BOOST_TEST_CASE(std::bind(test_equal, 1, 2, 1, 2, true)));
	basics->add(BOOST_TEST_CASE(std::bind(test_equal, 1, 2, 2, 2, false)));
	basics->add(BOOST_TEST_CASE(std::bind(test_equal, 1, 2, 1, 3, false)));
	basics->add(BOOST_TEST_CASE(std::bind(test_equal, 1, 2, 4, 3, false)));

	// != tests
	basics->add(BOOST_TEST_CASE(std::bind(test_not_equal, 1, 2, 3, 2, true)));
	basics->add(BOOST_TEST_CASE(std::bind(test_not_equal, 1, 2, 1, 3, true)));
	basics->add(BOOST_TEST_CASE(std::bind(test_not_equal, 3, 2, 3, 2, false)));

	// < tests
	basics->add(BOOST_TEST_CASE(std::bind(test_less, 1, 1, 1, 2, true)));
	basics->add(BOOST_TEST_CASE(std::bind(test_less, 1, 2, 2, 2, true)));
	basics->add(BOOST_TEST_CASE(std::bind(test_less, 1, 1, 5, 0, true)));
	basics->add(BOOST_TEST_CASE(std::bind(test_less, 1, 2, 1, 1, false)));
	basics->add(BOOST_TEST_CASE(std::bind(test_less, 2, 2, 1, 2, false)));
	basics->add(BOOST_TEST_CASE(std::bind(test_less, 5, 0, 1, 1, false)));
	basics->add(BOOST_TEST_CASE(std::bind(test_less, 5, 0, 5, 0, false)));

	// <= tests (same as less than, except for equal case)
	basics->add(BOOST_TEST_CASE(std::bind(test_less_equal, 1, 1, 1, 2, true)));
	basics->add(BOOST_TEST_CASE(std::bind(test_less_equal, 1, 2, 2, 2, true)));
	basics->add(BOOST_TEST_CASE(std::bind(test_less_equal, 1, 1, 5, 0, true)));
	basics->add(BOOST_TEST_CASE(std::bind(test_less_equal, 1, 2, 1, 1, false)));
	basics->add(BOOST_TEST_CASE(std::bind(test_less_equal, 2, 2, 1, 2, false)));
	basics->add(BOOST_TEST_CASE(std::bind(test_less_equal, 5, 0, 1, 1, false)));
	basics->add(BOOST_TEST_CASE(std::bind(test_less_equal, 5, 0, 5, 0, true)));

	// > tests (opposite results to <, except for equal case)
	basics->add(BOOST_TEST_CASE(std::bind(test_greater, 1, 1, 1, 2, false)));
	basics->add(BOOST_TEST_CASE(std::bind(test_greater, 1, 2, 2, 2, false)));
	basics->add(BOOST_TEST_CASE(std::bind(test_greater, 1, 1, 5, 0, false)));
	basics->add(BOOST_TEST_CASE(std::bind(test_greater, 1, 2, 1, 1, true)));
	basics->add(BOOST_TEST_CASE(std::bind(test_greater, 2, 2, 1, 2, true)));
	basics->add(BOOST_TEST_CASE(std::bind(test_greater, 5, 0, 1, 1, true)));
	basics->add(BOOST_TEST_CASE(std::bind(test_greater, 5, 0, 5, 0, false)));

	// >= tests (same results as >, except for equal case)
	basics->add(BOOST_TEST_CASE(std::bind(test_greater_equal, 1, 1, 1, 2, false)));
	basics->add(BOOST_TEST_CASE(std::bind(test_greater_equal, 1, 2, 2, 2, false)));
	basics->add(BOOST_TEST_CASE(std::bind(test_greater_equal, 1, 1, 5, 0, false)));
	basics->add(BOOST_TEST_CASE(std::bind(test_greater_equal, 1, 2, 1, 1, true)));
	basics->add(BOOST_TEST_CASE(std::bind(test_greater_equal, 2, 2, 1, 2, true)));
	basics->add(BOOST_TEST_CASE(std::bind(test_greater_equal, 5, 0, 1, 1, true)));
	basics->add(BOOST_TEST_CASE(std::bind(test_greater_equal, 5, 0, 5, 0, true)));

	return true;
}

int
main(int argc, char** argv)
{
	return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}
