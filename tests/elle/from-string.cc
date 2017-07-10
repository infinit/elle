#define ELLE_TEST_MODULE from_string
#include <elle/test.hh>

#include <elle/from-string.hh>

using elle::from_string;

BOOST_AUTO_TEST_CASE(bool_)
{
  BOOST_TEST(from_string<bool>("false") == false);
  BOOST_TEST(from_string<bool>("0")     == false);
  BOOST_TEST(from_string<bool>("no")    == false);

  BOOST_TEST(from_string<bool>("true") == true);
  BOOST_TEST(from_string<bool>("1")    == true);
  BOOST_TEST(from_string<bool>("yes")  == true);
}

namespace
{
  template <typename T>
  void
  check_number(T v)
  {
    BOOST_TEST(from_string<T>(std::to_string(v)) == v);
  }

  template <typename T>
  void
  check_limits()
  {
    check_number(std::numeric_limits<T>::min());
    elle::meta::static_if<std::is_signed<T>{}>
      ([](auto one)
      {
        // Signed.
        check_number(-one);
        check_number(T{42} * -one);
      },
       [](auto)
      {
        // Unsigned.
        BOOST_CHECK_THROW(from_string<T>("-12"),
                          std::out_of_range);
      })(T{1});
    check_number(T{0});
    check_number(T{42});
    check_number(std::numeric_limits<T>::max());

    BOOST_CHECK_THROW(from_string<T>("0crap"),
                      std::invalid_argument);
  }
}

BOOST_AUTO_TEST_CASE(numbers)
{
  BOOST_TEST(from_string<int>("42")      == 42);
  BOOST_TEST(from_string<int>("  +42")   == 42);
  BOOST_TEST(from_string<int>("-42")     == -42);
  BOOST_TEST(from_string<unsigned>("42") == 42u);
  BOOST_TEST(from_string<float>("42")    == 42.f);
  BOOST_TEST(from_string<double>("42")   == 42.);

  check_limits<int>();
  check_limits<long long>();
  check_limits<long>();
  check_limits<signed char>();
  check_limits<signed short>();
  check_limits<unsigned char>();
  check_limits<unsigned long long>();
  check_limits<unsigned long>();
  check_limits<unsigned short>();
  check_limits<unsigned>();
}
