#define BOOST_TEST_MODULE setenv

#include <boost/test/unit_test.hpp>
#include <elle/os/getenv.hh>
#include <elle/os/setenv.hh>
#include <elle/Exception.hh>

BOOST_AUTO_TEST_CASE(setenv1)
{
  std::string key = "hello";
  std::string val = "world";
  elle::os::setenv(key, val, 1);

  std::string res = elle::os::getenv(key);
  BOOST_CHECK(res == val);
}

BOOST_AUTO_TEST_CASE(setenv2)
{
  std::string key = "hello";
  std::string val = "world";
  std::string val2 = "world";
  elle::os::setenv(key, val, 0);
  elle::os::setenv(key, val2, 0);
  std::string res = elle::os::getenv(key);
  BOOST_CHECK(res == val);
}
