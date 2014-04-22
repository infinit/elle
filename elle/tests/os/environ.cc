#include <elle/Exception.hh>
#include <elle/os/environ.hh>
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

static
void
environ2()
{

  std::string key = "hello";
  std::string val = "world";

  os::setenv(key, val, 1);
  BOOST_CHECK(os::getenv(key) == val);
  os::unsetenv(key);
  auto environ = os::environ();
  BOOST_CHECK_THROW(environ.at(key), std::out_of_range);
}

static
void
getenv1()
{
  std::string prog = elle::os::getenv("PATH");
  BOOST_CHECK(!prog.empty());
}

static
void
getenv_missing()
{
  BOOST_REQUIRE_THROW(elle::os::getenv("hahaha not existing key="),
                                       elle::Exception);
}

static
void
getenv_default()
{
  std::string def = elle::os::getenv("cetteclefn'existepaslol", "(none)");
  BOOST_CHECK(def == "(none)");
}

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

  suite.add(BOOST_TEST_CASE(environ1));
  suite.add(BOOST_TEST_CASE(environ2));
  suite.add(BOOST_TEST_CASE(getenv1));
  suite.add(BOOST_TEST_CASE(getenv_missing));
  suite.add(BOOST_TEST_CASE(getenv_default));
  suite.add(BOOST_TEST_CASE(setenv1));
  suite.add(BOOST_TEST_CASE(setenv2));
}
