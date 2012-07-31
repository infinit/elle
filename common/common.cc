#include <pwd.h>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>

#include <elle/os.hh>
#include <elle/os/path.hh>
#include <elle/print.hh>
#include <elle/system/Platform.hh>

#include <elle/idiom/Close.hh>

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
        common::system::home_directory() + "/" + COMMON_DEFAULT_INFINIT_HOME
    );
  }

  std::string _built_binary_relative_path(std::string const& name)
  {
    static std::unordered_map<std::string, std::string> paths{
      {"8access",   "satellites/access/8access"},
      {"8infinit",  "8infinit"},
      {"8watchdog", "plasma/watchdog/8watchdog"},
    };
    auto it = paths.find(name);
    if (it == paths.end())
      throw std::runtime_error("Built binary '" + name + "' not registered");
    return it->second;
  }

  short _meta_port()
  {
    std::string port_string = elle::os::getenv(
        "INFINIT_META_PORT",
        elle::sprint(COMMON_DEFAULT_META_PORT)
    );
    std::stringstream ss(port_string);
    short port;
    ss >> port;
    if (ss.fail())
      throw std::runtime_error{
          "Couldn't retreive the port from '" + port_string + "'"
      };
    return port;
  }
}

namespace common
{

  namespace infinit
  {

    std::string const& home()
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
          path = elle::os::path::join(home(), "bin", name);

      if (ensure && !elle::os::path::exists(path))
        throw std::runtime_error("Cannot find any binary at '" + path + "'");

      return (binaries[name] = path);
    }

  }


  namespace system
  {

    std::string const& home_directory()
    {
      static std::string home_dir = _home_directory();
      return home_dir;
    }

    std::string const& platform()
    {
      static std::string const platform_ =
#ifdef INFINIT_LINUX
        "linux";
#else
# ifdef INFINIT_MACOSX
        "macosx";
# else
#  ifdef INFINIT_WINDOWS
        "windows";
#  endif
# endif
#endif
        return platform_;
    }

    unsigned int architecture()
    {
      return sizeof(void*) * 8;
    }

  } //!system

  namespace meta
  {

    std::string const& protocol()
    {
      static std::string const protocol = elle::os::getenv(
          "INFINIT_META_PROTOCOL"
          COMMON_DEFAULT_META_PROTOCOL
      );
      return protocol;
    }

    short port()
    {
      static short const port = _meta_port();
      return port;
    }

    std::string const& host()
    {
      static std::string const host = elle::os::getenv(
          "INFINIT_META_HOST",
          COMMON_DEFAULT_META_HOST
      );
      return host;
    }

    std::string const& url()
    {
      static std::string const url = elle::os::getenv(
          "INFINIT_META_URL",
          protocol() + "://" + host()
            + ":" + elle::sprint(port())
      );
      return url;
    }

  } // !meta

  namespace resources
  {

    std::string base_url(char const* platform_,
                         unsigned int architecture_)
    {
      static std::string const base_url = elle::os::getenv(
          "INFINIT_RESOURCES_ROOT_URL",
          COMMON_DEFAULT_RESOURCES_ROOT_URL
      );
      std::string platform = (
          platform_ != nullptr ?
          std::string(platform_) :
          ::common::system::platform()
      );
      std::string architecture = elle::sprint(
          architecture_ != 0 ?
          architecture_ :
          ::common::system::architecture()
      );
      return base_url + "/" + platform + architecture;
    }

    std::string manifest(char const* platform,
                         unsigned int architecture)
    {
      return base_url(platform, architecture) + "/manifest.xml";
    }

  }


  //- scheduled for deletion --------------------------------------------------

  std::string const& passport_path()
  {
    static std::string const path = elle::os::path::join(infinit::home(),
                                                         "infinit.ppt");
    return path;
  }

  namespace watchdog
  {

    std::string const& identity_path()
    {
      static std::string const path = elle::os::path::join(infinit::home(),
                                                           "identity.wtg");
      return path;
    }

    std::string const& server_name()
    {
      // The name is specific to the infinit home, as many instance could be
      // launched on the same machine.
      static std::string const name =
        ::common::infinit::home() + "/WATCHDOG_SERVER_NAME";
      return name;
    }

  }

}

