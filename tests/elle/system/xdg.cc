#define ELLE_TEST_MODULE elle_system_xdg

#include <elle/test.hh>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem.hpp>

#include <elle/system/user_paths.hh>
#include <elle/system/username.hh>
#include <elle/filesystem/TemporaryDirectory.hh>

#include <elle/log.hh>
#include <elle/os/environ.hh>
#include <elle/system/XDG.hh>

namespace bfs = ::boost::filesystem;
using namespace std::literals;

namespace suffixes
{
  static
  bfs::path
  cache(std::string const& company, std::string const& product)
  {
    return bfs::path(".cache"s) / company / product;
  }

  static
  bfs::path
  data(std::string const& company, std::string const& product)
  {
    return bfs::path(".local"s) / "share" / company / product;
  }

  static
  bfs::path
  state(std::string const& company, std::string const& product)
  {
    return bfs::path(".local"s) / "state" / company / product;
  }

  static
  bfs::path
  config(std::string const& company, std::string const& product)
  {
    return bfs::path(".config"s) / company / product;
  }
}

static auto const company = "infinit";
static auto const product = "elle";

static
void
test(elle::system::XDG const& xdg,
     bfs::path const& home)
{
  BOOST_CHECK_EQUAL(xdg.cache_dir(), home / suffixes::cache(company, product));
  BOOST_CHECK_EQUAL(xdg.data_dir(), home / suffixes::data(company, product));
  BOOST_CHECK_EQUAL(xdg.state_dir(), home / suffixes::state(company, product));
  BOOST_CHECK_EQUAL(xdg.config_dir(), home / suffixes::config(company, product));
  // XXX: Temporary and runtime.
}

BOOST_AUTO_TEST_CASE(base)
{
  auto xdg = elle::system::XDG(company, product);
  BOOST_CHECK_EQUAL(xdg.home_dir(), elle::system::home_directory());
  test(xdg, elle::system::home_directory());
}

BOOST_AUTO_TEST_CASE(global_env)
{
  elle::filesystem::TemporaryDirectory d;
  auto const path = bfs::canonical(d.path()).string();
  elle::os::setenv("ELLE_HOME", path);
  auto xdg = elle::system::XDG("infinit", "elle");
  test(xdg, path);
}

BOOST_AUTO_TEST_CASE(specific_env)
{
  elle::filesystem::TemporaryDirectory d;
  auto const path = bfs::canonical(d.path()).string();
  auto set = [&] (std::string var) {
    elle::os::setenv(var, path);
    return elle::system::XDG(company, product);
  };
  BOOST_CHECK_EQUAL(set("ELLE_DATA_HOME").data_dir(), path);
  BOOST_CHECK_EQUAL(set("ELLE_CACHE_HOME").cache_dir(), path);
  BOOST_CHECK_EQUAL(set("ELLE_STATE_HOME").state_dir(), path);
  BOOST_CHECK_EQUAL(set("ELLE_CONFIG_HOME").config_dir(), path);
  BOOST_CHECK_EQUAL(set("ELLE_RUNTIME_DIR").runtime_dir(), path);
  BOOST_CHECK_EQUAL(set("TMPDIR").tmp_dir(), path);
}
