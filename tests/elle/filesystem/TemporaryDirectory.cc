#include <elle/test.hh>
#include <elle/filesystem/TemporaryDirectory.hh>

using elle::filesystem::TemporaryDirectory;

static
void
simple()
{
  boost::filesystem::path path;
  {
    TemporaryDirectory d;
    path = d.path();
    BOOST_CHECK(is_directory(path));
  }
  BOOST_CHECK(!exists(path));
}

static
void
named()
{
  boost::filesystem::path path;
  {
    TemporaryDirectory d("some-name");
    path = d.path();
    BOOST_CHECK_EQUAL(path.filename(), "some-name");
    BOOST_CHECK(is_directory(path));
  }
  BOOST_CHECK(!exists(path));
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(simple));
  suite.add(BOOST_TEST_CASE(named));
}
