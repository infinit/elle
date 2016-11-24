#include "cryptography.hh"

#include <cryptography/random.hh>

/*--------.
| Operate |
`--------*/

static
void
test_operate_boolean()
{
  infinit::cryptography::random::generate<bool>();
}

static
void
test_operate_real()
{
  double value1 =
    infinit::cryptography::random::generate<double>();
  double value2 =
    infinit::cryptography::random::generate<double>();
  // With very high probability.
  BOOST_CHECK_NE(value1, value2);
}

template <typename T>
void
test_operate_x(T minimum,
               T maximum)
{
  infinit::cryptography::random::generate<T>();
  T value = infinit::cryptography::random::generate<T>(minimum, maximum);
  BOOST_CHECK_GE(value, minimum);
  BOOST_CHECK_LE(value, maximum);
}

static
void
test_operate_string()
{
  std::string value1 =
    infinit::cryptography::random::generate<std::string>(262);
  std::string value2 =
    infinit::cryptography::random::generate<std::string>(262);
  // With very high probability.
  std::cerr << std::hex;
  BOOST_CHECK_NE(elle::ConstWeakBuffer(value1), elle::ConstWeakBuffer(value2));
}

static
void
test_operate_buffer()
{
  elle::Buffer value1 =
    infinit::cryptography::random::generate<elle::Buffer>(262);
  elle::Buffer value2 =
    infinit::cryptography::random::generate<elle::Buffer>(262);
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
  test_operate_real();
  test_operate_x<int8_t>(-60, -58);
  test_operate_x<int16_t>(-21000, 21000);
  test_operate_x<int32_t>(-848, 73435);
  test_operate_x<int64_t>(-324923, 32212394);
  test_operate_x<uint8_t>(75, 126);
  test_operate_x<uint16_t>(1238, 53104);
  test_operate_x<uint32_t>(424242, 424242);
  test_operate_x<uint64_t>(23409, 1209242094821);
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
