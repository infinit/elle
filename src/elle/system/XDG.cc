#include <boost/filesystem/exception.hpp>
#include <boost/filesystem.hpp>

#include <elle/system/user_paths.hh>
#include <elle/system/username.hh>
#include <elle/err.hh>

#include <elle/log.hh>
#include <elle/os/environ.hh>
#include <elle/system/XDG.hh>

ELLE_LOG_COMPONENT("elle.system.xdg");

namespace elle
{
  namespace system
  {
    namespace bfs = ::boost::filesystem;
    using namespace std::literals;

    namespace
    {
      static
      std::string
      upper(std::string str)
      {
        std::transform(str.begin(), str.end(), str.begin(),
                       [] (auto const c) { return std::toupper(c); });
        return str;
      }
    }

    XDG::XDG(std::string const& company,
             std::string const& product,
             boost::optional<std::string> const& prefix)
      : _company(company)
      , _product(product)
      , _environment_variable_prefix(upper(prefix.get_value_or(product)))
    {}

    bfs::path
    XDG::home_dir() const
    {
      auto const home =
        os::getenv(this->_environment_variable_prefix + "_HOME", ""s);
      return home.empty() ? home_directory() : home;
    }

    bfs::path
    XDG::_xdg(std::string const& type,
              bfs::path const& def) const
    {
      auto const from_env = elle::os::getenv(
        this->_environment_variable_prefix + "_" + type, ""s);
      auto const xdg = elle::os::getenv("XDG_" + type, ""s);
      auto const dir = !from_env.empty()
        ? from_env
        :!xdg.empty()
          ? bfs::path(xdg) / this->_company / this->_product
          : def;
      try
      {
        if (bfs::exists(dir) && !bfs::is_directory(dir))
          elle::err("not a directory: %s", dir.string());
        bfs::create_directories(dir);
        boost::system::error_code erc;
        bfs::permissions(dir, bfs::add_perms | bfs::owner_write, erc);
        return bfs::canonical(dir);
      }
      catch (bfs::filesystem_error& e)
      {
        std::string env = !from_env.empty()
          ? std::string{this->_environment_variable_prefix + "_"}
          : !xdg.empty()
            ? std::string{"XDG_"}
            : std::string{};
        if (!env.empty())
          ELLE_WARN("Invalid \"%s%s\" directory: %s", env, type, e.what());
        elle::err(e.what());
      }
    }

    bfs::path
    XDG::_xdg_home(std::string const& type,
                   bfs::path const& def) const
    {
      return this->_xdg(
        type + "_HOME",
        this->home_dir() / def / this->_company / this->_product);
    }

    bfs::path
    XDG::cache_dir() const
    {
      return this->_xdg_home("CACHE", ".cache");
    }

    bfs::path
    XDG::config_dir() const
    {
      return this->_xdg_home("CONFIG", ".config");
    }

    bfs::path
    XDG::data_dir() const
    {
      return this->_xdg_home("DATA", ".local/share");
    }

    bfs::path
    XDG::tmp_dir() const
    {
      return elle::os::getenv("TMPDIR", "/tmp");;
    }

    bfs::path
    XDG::runtime_dir(boost::optional<std::string> fallback) const
    {
      return this->_xdg(
        "RUNTIME_DIR",
        fallback
        ? *fallback
        : this->tmp_dir() / this->_product / elle::system::username());
    }

    bfs::path
    XDG::state_dir() const
    {
      return this->_xdg_home("STATE", ".local/state");
    }
  }
}
