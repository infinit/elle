#include <elle/system/platform.hh>
#include <elle/printf.hh>
#ifdef ELLE_WINDOWS
# include <elle/system/Windows/version.hh>
#endif

namespace elle
{
  namespace system
  {
    namespace platform
    {
      std::string
      os_name()
      {
#if defined ELLE_WINDOWS
        return "Windows";
#elif defined ELLE_LINUX
        return "Linux";
#elif defined ELLE_ANDROID
        return "Android";
#elif defined ELLE_MACOS
        return "MacOSX";
#elif defined ELLE_IOS
        return "iOS";
#else
# error Please define ELLE_{OS} according to your platform.
#endif
      }

      std::string
      os_version()
      {
#if defined ELLE_WINDOWS
        static const std::string version = windows::version_name();
        return version;
#else
        return "unknown";
#endif
      }

      std::string
      os_description()
      {
        return elle::sprintf("%s %s", os_name(), os_version());
      }
    }
  }
}
