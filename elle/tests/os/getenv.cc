#define BOOST_TEST_MODULE cast
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <elle/os/getenv.hh>
#include <elle/Exception.hh>

BOOST_AUTO_TEST_CASE(getenv1)
{
  std::string prog = elle::os::getenv("_");
  BOOST_CHECK(!prog.empty());
}

BOOST_AUTO_TEST_CASE(getenv_missing)
{
  BOOST_REQUIRE_THROW(elle::os::getenv("hahaha not existing key="),
                                       elle::Exception);
}

BOOST_AUTO_TEST_CASE(getenv_default)
{
  std::string def = elle::os::getenv("cetteclefn'existepaslol", "(none)");
  BOOST_CHECK(def == "(none)");
}
