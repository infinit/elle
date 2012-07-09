#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <elle/os/getenv.hh>

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

}

