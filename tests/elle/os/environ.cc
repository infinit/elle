#include <elle/Exception.hh>
#include <elle/algorithm.hh>
#include <elle/os/environ.hh>
#include <elle/test.hh>

namespace os = elle::os;

static
void
environ1()
{
  std::string const key = "hello";
  std::string const val = "world";

  os::setenv(key, val);
  auto environ = os::environ();
  BOOST_TEST(elle::contains(environ, key));
  BOOST_TEST(environ.find(key)->second == val);
  BOOST_TEST(environ.at(key) == val);
}

static
void
environ2()
{
  std::string const key = "hello";
  std::string const val = "world";

  os::setenv(key, val);
  BOOST_TEST(os::getenv(key) == val);
  os::unsetenv(key);
  BOOST_CHECK_THROW(os::environ().at(key), std::out_of_range);
}

static
void
getenv_()
{
  BOOST_TEST(!elle::os::getenv("PATH").empty());

  BOOST_REQUIRE_THROW(elle::os::getenv("hahaha not existing key="),
                                       elle::Exception);

  {
    auto const k = "cetteclefn'existepaslol";
    BOOST_TEST(elle::os::getenv(k, "(none)") == "(none)");
    BOOST_TEST(elle::os::getenv(k, "42") == "42");
    BOOST_TEST(elle::os::getenv(k, 42) == 42);
  }
}

static
void
setenv1()
{
  std::string const key = "hello";
  std::string const val = "world";
  elle::os::setenv(key, val, true);
  BOOST_TEST(elle::os::getenv(key) == val);
}

static
void
setenv2()
{
  std::string const key = "hello";
  std::string const val = "world";
  std::string const val2 = "WORLD";
  elle::os::setenv(key, val, false);
  elle::os::setenv(key, val2, false);
  BOOST_TEST(elle::os::getenv(key) == val);
  elle::os::setenv(key, val2);
  BOOST_TEST(elle::os::getenv(key) == val2);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();

  suite.add(BOOST_TEST_CASE(environ1));
  suite.add(BOOST_TEST_CASE(environ2));
  suite.add(BOOST_TEST_CASE(getenv_));
  suite.add(BOOST_TEST_CASE(setenv1));
  suite.add(BOOST_TEST_CASE(setenv2));
}
