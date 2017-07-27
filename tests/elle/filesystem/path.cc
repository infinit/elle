#define BOOST_TEST_MODULE path

#include <elle/filesystem/TemporaryFile.hh>
#include <elle/filesystem/path.hh>
#include <elle/fstream.hh>
#include <elle/test.hh>

namespace fs = boost::filesystem;

BOOST_AUTO_TEST_CASE(base)
{
  for (auto const& name: {"foo", "foo.", "foo.1bar"})
  {
    auto foo = elle::filesystem::TemporaryFile{name};
    auto&& f = elle::ofstream(foo.path());
    BOOST_CHECK_THROW(f.base(), elle::Error);
  }
  for (auto const& name: {"foo.0", "foo.12", "foo.1345"})
  {
    auto foo = elle::filesystem::TemporaryFile{name};
    auto&& f = elle::ofstream(foo.path());
    BOOST_CHECK_EQUAL(f.base(), foo.directory().path() / "foo");
  }
}
