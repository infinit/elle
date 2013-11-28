#include <elle/Exception.hh>
#include <elle/os/getenv.hh>
#include <elle/test.hh>

static
void
getenv1()
{
  std::string prog = elle::os::getenv("_");
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

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();

  suite.add(BOOST_TEST_CASE(getenv1));
  suite.add(BOOST_TEST_CASE(getenv_missing));
  suite.add(BOOST_TEST_CASE(getenv_default));
}
