#include <pwd.h>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>

#include <elle/os.hh>
#include <elle/os/path.hh>

#include <common/common.hh>

namespace
{

  std::string _home_directory()
  {
    struct passwd* pw = ::getpwuid(getuid());
    if (pw != nullptr && pw->pw_dir != nullptr)
      return std::string{pw->pw_dir};
    else
      return elle::os::getenv("HOME", "/tmp");
  }

  std::string _infinit_home()
  {
    return elle::os::getenv(
        "INFINIT_HOME",
        common::home_directory() + "/" + COMMON_DEFAULT_INFINIT_HOME
    );
  }

  std::string _built_binary_relative_path(std::string const& name)
  {
    static std::unordered_map<std::string, std::string> paths{
      {"8access",   "satellites/8access"},
      {"8infinit",  "8infinit"},
      {"8watchdog", "plasma/watchdog/8watchdog"},
    };
    auto it = paths.find(name);
    if (it == paths.end())
      throw std::runtime_error("Built binary '" + name + "' not registered");
    return it->second;
  }

}

namespace common
{

  std::string const& home_directory()
  {
    static std::string home_dir = _home_directory();
    return home_dir;
  }

  std::string const& infinit_home()
  {
    static std::string infinit_dir = _infinit_home();
    return infinit_dir;
  }

  std::string const& binary_path(std::string const& name, bool ensure)
  {
    static std::unordered_map<std::string, std::string> binaries;

    auto it = binaries.find(name);
    if (it != binaries.end())
      return it->second;

    static std::string build_dir = elle::os::getenv("INFINIT_BUILD_DIR", "");
    std::string path;
    if (build_dir.size())
        path = elle::os::path::join(build_dir,
                                    _built_binary_relative_path(name));
    else
        path = elle::os::path::join(infinit_home(), "bin", name);

    if (ensure && !elle::os::path::exists(path))
      throw std::runtime_error("Cannot find any binary at '" + path + "'");

    return (binaries[name] = path);
  }

}

