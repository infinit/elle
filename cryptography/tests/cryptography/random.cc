#include "cryptography.hh"

#include <cryptography/random.hh>
#include <cryptography/Exception.hh>

/*--------.
| Operate |
`--------*/

static
void
test_operate_boolean()
{
  infinit::cryptography::random::generate<elle::Boolean>();
}

static
void
test_operate_real()
{
  elle::Real value1 =
    infinit::cryptography::random::generate<elle::Real>();
  elle::Real value2 =
    infinit::cryptography::random::generate<elle::Real>();

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
  elle::String value1 =
    infinit::cryptography::random::generate<elle::String>(262);
  elle::String value2 =
    infinit::cryptography::random::generate<elle::String>(262);

  // With very high probability.
  BOOST_CHECK_NE(value1, value2);
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
  BOOST_CHECK_NE(value1, value2);
}

static
void
test_operate()
{
  test_operate_boolean();
  test_operate_x<elle::Character>(15, 48);
  test_operate_real();
  test_operate_x<elle::Integer8>(-60, -58);
  test_operate_x<elle::Integer16>(-21000, 21000);
  test_operate_x<elle::Integer32>(-848, 73435);
  test_operate_x<elle::Integer64>(-324923, 32212394);
  test_operate_x<elle::Natural8>(75, 126);
  test_operate_x<elle::Natural16>(1238, 53104);
  test_operate_x<elle::Natural32>(424242, 424242);
  test_operate_x<elle::Natural64>(23409, 1209242094821);
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

