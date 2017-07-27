#define BOOST_TEST_MODULE TemporaryDirectory

#include <elle/test.hh>
#include <elle/filesystem/TemporaryDirectory.hh>

using elle::filesystem::TemporaryDirectory;

namespace fs = boost::filesystem;

BOOST_AUTO_TEST_CASE(simple)
{
  auto path = fs::path{};
  {
    TemporaryDirectory d;
    path = d.path();
    BOOST_CHECK(is_directory(path));
  }
  BOOST_CHECK(!exists(path));
}

BOOST_AUTO_TEST_CASE(named)
{
  fs::path path;
  {
    TemporaryDirectory d("some-name");
    path = d.path();
    BOOST_CHECK_EQUAL(path.filename(), "some-name");
    BOOST_CHECK(is_directory(path));
  }
  BOOST_CHECK(!exists(path));
}
