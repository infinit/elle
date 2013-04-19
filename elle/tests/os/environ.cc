#define BOOST_TEST_MODULE environ
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <elle/os/environ.hh>
#include <elle/os/setenv.hh>
#include <elle/os/getenv.hh>
#include <elle/Exception.hh>

namespace os = elle::os;

BOOST_AUTO_TEST_CASE(environ1)
{
  std::string key = "hello";
  std::string val = "world";

  os::setenv(key, val, 1);
  auto environ = os::environ();
  BOOST_CHECK(environ.find(key) != end(environ));
  auto it = environ.find(key);
  BOOST_CHECK(it->second == val);
}
