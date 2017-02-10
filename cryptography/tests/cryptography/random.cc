#include "cryptography.hh"

#include <boost/type_index.hpp>

#include <cryptography/random.hh>

// On some platforms "random" exists:
//
// /usr/include/stdlib.h:321:17: note: previous declaration 'long int random()'
// extern long int random (void) __THROW;
//                 ^
namespace icrand = elle::cryptography::random;

/*--------.
| Operate |
`--------*/

static
void
test_operate_boolean()
{
  icrand::generate<bool>();
}

static
void
test_operate_real()
{
  double value1 = icrand::generate<double>();
  double value2 = icrand::generate<double>();
  // With very high probability.
  BOOST_CHECK_NE(value1, value2);
}

template <typename T>
void
test_operate_x(T min, T max)
{
  BOOST_TEST_MESSAGE("Testing T = "
                     << boost::typeindex::type_id<T>().pretty_name());
  {
    auto min = std::numeric_limits<T>::min();
    auto max = std::numeric_limits<T>::max();
    auto val = icrand::generate<T>();
    BOOST_TEST_MESSAGE("min = " << +min);
    BOOST_TEST_MESSAGE("val = " << +val);
    BOOST_TEST_MESSAGE("max = " << +max);
    BOOST_CHECK_LE(std::numeric_limits<T>::min(), val);
    BOOST_CHECK_LE(val, std::numeric_limits<T>::max());

  }
  {
    T val = icrand::generate<T>(min, max);
    BOOST_TEST_MESSAGE("min = " << +min);
    BOOST_TEST_MESSAGE("val = " << +val);
    BOOST_TEST_MESSAGE("max = " << +max);
    BOOST_CHECK_LE(min, val);
    BOOST_CHECK_LE(val, max);
  }
}

static
void
test_operate_string()
{
  std::string value1 = icrand::generate<std::string>(262);
  std::string value2 = icrand::generate<std::string>(262);
  // With very high probability.
  std::cerr << std::hex;
  BOOST_CHECK_NE(elle::ConstWeakBuffer(value1), elle::ConstWeakBuffer(value2));
}

static
void
test_operate_buffer()
{
  elle::Buffer value1 = icrand::generate<elle::Buffer>(262);
  elle::Buffer value2 = icrand::generate<elle::Buffer>(262);
  // With very high probability.
  std::cerr << std::hex;
  BOOST_CHECK_NE(value1, value2);
}

static
void
test_operate()
{
  test_operate_boolean();
  test_operate_x<char>(15, 48);
  test_operate_x<int8_t>(-60, -58);
  test_operate_x<int16_t>(-21000, 21000);
  test_operate_x<int32_t>(-848, 73435);
  test_operate_x<int64_t>(-324923, 32212394);
  test_operate_x<uint8_t>(75, 126);
  test_operate_x<uint16_t>(1238, 53104);
  test_operate_x<uint32_t>(424242, 424242);
  test_operate_x<uint64_t>(23409, 1209242094821);
  test_operate_real();
  test_operate_string();
  test_operate_buffer();
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("random");
  suite->add(BOOST_TEST_CASE(test_operate));
  boost::unit_test::framework::master_test_suite().add(suite);
}
