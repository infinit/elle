#include <elle/Exception.hh>
#include <elle/os/getenv.hh>
#include <elle/os/setenv.hh>
#include <elle/test.hh>

static
void
setenv1()
{
  std::string key = "hello";
  std::string val = "world";
  elle::os::setenv(key, val, 1);

  std::string res = elle::os::getenv(key);
  BOOST_CHECK(res == val);
}

static
void
setenv2()
{
  std::string key = "hello";
  std::string val = "world";
  std::string val2 = "world";
  elle::os::setenv(key, val, 0);
  elle::os::setenv(key, val2, 0);
  std::string res = elle::os::getenv(key);
  BOOST_CHECK(res == val);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(setenv1));
  suite.add(BOOST_TEST_CASE(setenv2));
}

