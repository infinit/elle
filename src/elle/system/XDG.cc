#include <filesystem>

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
    using namespace std::literals;
    namespace fs = std::filesystem;

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
             std::optional<std::string> const& prefix)
      : _company(company)
      , _product(product)
      , _environment_variable_prefix(upper(prefix.value_or(product)))
    {}

    fs::path
    XDG::home_dir() const
    {
      auto const home =
        os::getenv(this->_environment_variable_prefix + "_HOME", ""s);
      return home.empty() ? home_directory() : fs::path(home);
    }

    fs::path
    XDG::_xdg(std::string const& type,
              fs::path const& def) const
    {
      auto const from_env = elle::os::getenv(
        this->_environment_variable_prefix + "_" + type, ""s);
      auto const xdg = elle::os::getenv("XDG_" + type, ""s);
      auto const dir = !from_env.empty() ?
        fs::path(from_env) :
        !xdg.empty() ?
        fs::path(xdg) / this->_company / this->_product : def;
      try
      {
        if (exists(dir) && !is_directory(dir))
          elle::err("not a directory: %s", dir.string());
        create_directories(dir);
        std::error_code erc;
        permissions(dir, fs::perms::owner_write, fs::perm_options::add, erc);
        return canonical(dir);
      }
      catch (fs::filesystem_error const& e)
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

    fs::path
    XDG::_xdg_home(std::string const& type,
                   fs::path const& def) const
    {
      return this->_xdg(
        type + "_HOME",
        this->home_dir() / def / this->_company / this->_product);
    }

    fs::path
    XDG::cache_dir() const
    {
      return this->_xdg_home("CACHE", ".cache");
    }

    fs::path
    XDG::config_dir() const
    {
      return this->_xdg_home("CONFIG", ".config");
    }

    fs::path
    XDG::data_dir() const
    {
      return this->_xdg_home("DATA", ".local/share");
    }

    fs::path
    XDG::tmp_dir() const
    {
      return elle::os::getenv("TMPDIR", "/tmp");;
    }

    fs::path
    XDG::runtime_dir(std::optional<std::string> fallback) const
    {
      return this->_xdg(
        "RUNTIME_DIR",
        fallback ? fs::path(*fallback) :
        this->tmp_dir() / this->_product / elle::system::username());
    }

    fs::path
    XDG::state_dir() const
    {
      return this->_xdg_home("STATE", ".local/state");
    }
  }
}
