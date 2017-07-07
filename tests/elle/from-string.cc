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
