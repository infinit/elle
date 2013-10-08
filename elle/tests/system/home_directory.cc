#define BOOST_TEST_MODULE home_directory
#include <elle/test.hh>
#include <elle/system/home_directory.hh>

BOOST_AUTO_TEST_CASE(size)
{
  BOOST_CHECK(elle::system::home_directory().string().size() > 0);
}

