#define ELLE_TEST_MODULE from_string
#include <elle/test.hh>

#include <elle/from-string.hh>

BOOST_AUTO_TEST_CASE(bool_)
{
  using elle::from_string;
  BOOST_TEST(from_string<bool>("false") == false);
  BOOST_TEST(from_string<bool>("0")     == false);
  BOOST_TEST(from_string<bool>("no")    == false);

  BOOST_TEST(from_string<bool>("true") == true);
  BOOST_TEST(from_string<bool>("1")    == true);
  BOOST_TEST(from_string<bool>("yes")  == true);
}

BOOST_AUTO_TEST_CASE(numbers)
{
  using elle::from_string;
  BOOST_TEST(from_string<int>("42")      == 42);
  BOOST_TEST(from_string<int>("  +42")   == 42);
  BOOST_TEST(from_string<int>("-42")     == -42);
  BOOST_TEST(from_string<unsigned>("42") == 42u);
  BOOST_TEST(from_string<float>("42")    == 42.f);
  BOOST_TEST(from_string<double>("42")   == 42.);

  BOOST_CHECK_THROW(from_string<unsigned>("-12"),
                    std::out_of_range);
}
