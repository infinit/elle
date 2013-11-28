#include <elle/Exception.hh>
#include <elle/os/environ.hh>
#include <elle/os/setenv.hh>
#include <elle/os/getenv.hh>
#include <elle/test.hh>

namespace os = elle::os;

static
void
environ1()
{
  std::string key = "hello";
  std::string val = "world";

  os::setenv(key, val, 1);
  auto environ = os::environ();
  BOOST_CHECK(environ.find(key) != end(environ));
  auto it = environ.find(key);
  BOOST_CHECK(it->second == val);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();

  suite.add(BOOST_TEST_CASE(environ1));
}
