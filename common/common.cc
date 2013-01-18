#include <pwd.h>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>

#include <elle/os.hh>
#include <elle/os/path.hh>
#include <elle/print.hh>
#include <elle/system/platform.hh>
#include <elle/io/Path.hh>
#include <elle/io/Piece.hh>

#include <elle/assert.hh>

#include <elle/idiom/Close.hh>

#include <common/common.hh>

namespace path = elle::os::path;

namespace
{

  std::string
  _home_directory()
  {
    struct passwd* pw = ::getpwuid(getuid());
    if (pw != nullptr && pw->pw_dir != nullptr)
      return std::string{pw->pw_dir};
    else
      return elle::os::getenv("HOME", "/tmp");
  }

  std::string
  _download_directory()
  {
    std::string download_dir = elle::os::getenv("INFINIT_DOWNLOAD_DIR", "");
    if (download_dir.length() > 0 && path::exists(download_dir) && path::is_directory(download_dir))
      return download_dir;

    std::string home_dir = _home_directory();
    std::string probable_download_dir = path::join(home_dir, "/Downloads");

    if (path::exists(probable_download_dir) && path::is_directory(probable_download_dir))
      return probable_download_dir;

    return home_dir;
  }

  std::string
  _infinit_home()
  {
    return elle::os::getenv(
        "INFINIT_HOME",
        common::system::home_directory() + "/" + COMMON_DEFAULT_INFINIT_HOME
    );
  }

  std::string
  _built_binary_relative_path(std::string const& name)
  {
    static std::unordered_map<std::string, std::string> paths{
      {"8access",   "bin/8access"},
      {"8group",    "bin/8group"},
      {"8infinit",  "bin/8infinit"},
      {"8watchdog", "bin/8watchdog"},
      {"8transfer", "bin/8transfer"},
      {"8progess",  "bin/8progress"},
    };
    auto it = paths.find(name);
    if (it == paths.end())
      throw std::runtime_error("Built binary '" + name + "' not registered");
    return it->second;
  }

  uint16_t
  _meta_port()
  {
    std::string port_string = elle::os::getenv(
        "INFINIT_META_PORT",
        elle::sprint(COMMON_DEFAULT_META_PORT)
    );
    std::stringstream ss(port_string);
    uint16_t port;
    ss >> port;
    if (ss.fail())
      throw std::runtime_error{
          "Couldn't retreive the port from '" + port_string + "'"
      };
    return port;
  }

  uint16_t
  _trophonius_port()
  {
    std::string port_string = elle::os::getenv(
        "INFINIT_TROPHONIUS_PORT",
        elle::sprint(COMMON_DEFAULT_TROPHONIUS_PORT)
    );
    std::stringstream ss(port_string);
    uint16_t port;
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

    std::string const&
    home()
    {
      static std::string infinit_dir = _infinit_home();
      return infinit_dir;
    }

    std::string const&
    binary_path(std::string const& name, bool ensure)
    {
      static std::unordered_map<std::string, std::string> binaries;

      auto it = binaries.find(name);
      if (it != binaries.end())
        return it->second;

      static std::string build_dir = elle::os::getenv("INFINIT_BUILD_DIR", "");
      static std::string bin_dir = elle::os::getenv("INFINIT_BINARY_DIR", "");
      std::string path;
      if (build_dir.size())
          path = elle::os::path::join(build_dir,
                                      _built_binary_relative_path(name));
      else if (bin_dir.size())
          path = elle::os::path::join(bin_dir, name);
      else
          path = elle::os::path::join(home(), "bin", name);

      if (ensure && !elle::os::path::exists(path))
        throw std::runtime_error("Cannot find any binary at '" + path + "'");

      return (binaries[name] = path);
    }

    std::string
    network_directory(std::string const& user_id,
                      std::string const& network_id)
    {
      return path::join(
          user_directory(user_id),
          "networks",
          network_id
      );
    }

    std::string
    portal_path(std::string const& user_id,
                std::string const& network_id)
    {
      return path::join(network_directory(user_id, network_id), "portal.phr");
    }

    std::string
    user_directory(std::string const& user_id)
    {
      return path::join(home(), "users", user_id);
    }

  }


  namespace system
  {

    std::string const&
    home_directory()
    {
      static std::string home_dir = _home_directory();
      return home_dir;
    }

    std::string const&
    platform()
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

    unsigned int
    architecture()
    {
      return sizeof(void*) * 8;
    }

    std::string const&
    download_directory()
    {
      static std::string download_dir = _download_directory();
      return download_dir;
    }

  } //!system

  namespace meta
  {

    std::string const&
    protocol()
    {
      static std::string const protocol = elle::os::getenv(
          "INFINIT_META_PROTOCOL"
          COMMON_DEFAULT_META_PROTOCOL
      );
      return protocol;
    }

    uint16_t
    port()
    {
      static uint16_t const port = _meta_port();
      return port;
    }

    std::string const&
    host()
    {
      static std::string const host = elle::os::getenv(
          "INFINIT_META_HOST",
          COMMON_DEFAULT_META_HOST
      );
      return host;
    }

    std::string const&
    url()
    {
      static std::string const url = elle::os::getenv(
          "INFINIT_META_URL",
          protocol() + "://" + host()
            + ":" + elle::sprint(port())
      );
      return url;
    }

  } // !meta

  namespace trophonius
  {

    std::string const&
    protocol()
    {
      static std::string const protocol = elle::os::getenv(
          "INFINIT_TROPHONIUS_PROTOCOL"
          COMMON_DEFAULT_TROPHONIUS_PROTOCOL
      );
      return protocol;
    }

    uint16_t
    port()
    {
      static uint16_t const port = _trophonius_port();
      return port;
    }

    std::string const&
    host()
    {
      static std::string const host = elle::os::getenv(
          "INFINIT_TROPHONIUS_HOST",
          COMMON_DEFAULT_TROPHONIUS_HOST
      );
      return host;
    }

    std::string const&
    url()
    {
      static std::string const url = elle::os::getenv(
          "INFINIT_TROPHONIUS_URL",
          protocol() + "://" + host()
            + ":" + elle::sprint(port())
      );
      return url;
    }
  } // !trophonius

  namespace resources
  {

    std::string
    base_url(char const* platform_,
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

    std::string
    manifest(char const* platform,
             unsigned int architecture)
    {
      return base_url(platform, architecture) + "/manifest.xml";
    }

  }


  //- scheduled for deletion --------------------------------------------------

  std::string
  passport_path(std::string const& user)
  {
    return path::join(infinit::user_directory(user), user + ".ppt");
  }

  namespace watchdog
  {

    std::string
    server_name(std::string const& user_id)
    {
      return path::join(infinit::user_directory(user_id), "server.wtg");
    }

    std::string
    identity_path(std::string const& user_id)
    {
        return path::join(
          infinit::user_directory(user_id),
          "identity.wtg"
        );
    }

    std::string
    lock_path(std::string const& user_id)
    {
        return path::join(
          infinit::user_directory(user_id),
          "lock.wtg"
        );
    }

    std::string
    log_path(std::string const& user_id)
    {
        return path::join(
          infinit::user_directory(user_id),
          "log.wtg"
        );
    }

    std::string
    id_path(std::string const& user_id)
    {
        return path::join(
          infinit::user_directory(user_id),
          "id.wtg"
        );
    }
  }

  namespace longinus
  {
    std::string const&
    host()
    {
      static std::string const host_string = elle::os::getenv(
          "INFINIT_LONGINUS_HOST",
          elle::sprint(COMMON_DEFAULT_LONGINUS_HOST)
      );

      return host_string;
    }

    int
    port()
    {
      static std::string const port_string = elle::os::getenv(
          "INFINIT_LONGINUS_PORT",
          elle::sprint(COMMON_DEFAULT_LONGINUS_PORT)
      );
      return std::stoi(port_string);
    }
  }

  namespace metrics
  {
    std::string const&
    id_path()
    {
      static std::string const id_path = path::join(common::infinit::home(),
                                                    "ga.id");
      return id_path;
    }

    std::string const&
    google_server()
    {
      static std::string const analytics = "www.google-analytics.com";

      return analytics;
    }
  }
}
