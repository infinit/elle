#include <elle/system/platform.hh>
#include <elle/printf.hh>
#ifdef INFINIT_MACOSX
# include <CoreServices/CoreServices.h>
#elif defined INFINIT_WINDOWS
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
#if defined INFINIT_WINDOWS
        return "Windows";
#elif defined INFINIT_LINUX
        return "Linux";
#elif defined INFINIT_MACOSX
        return "MacOSX";
#elif defined INFINIT_IOS
        return "iOS";
#elif defined(INFINIT_ANDROID)
        return "Android";
#else
# error Please define INFINIT_{OS} according to your platform.
#endif
      }

      std::string
      os_version()
      {
#if defined INFINIT_WINDOWS
        static const std::string version = windows::version_name();
        return version;
#elif defined INFINIT_LINUX
        return "unknown";
#elif defined INFINIT_MACOSX
        int32_t major_version, minor_version, bugfix_version;
        if (Gestalt(gestaltSystemVersionMajor, &major_version) != noErr)
          return "unknown";
        if (Gestalt(gestaltSystemVersionMinor, &minor_version) != noErr)
          return "unknown";
        if (Gestalt(gestaltSystemVersionBugFix, &bugfix_version) != noErr)
          return "unknown";
        return elle::sprintf("%s.%s.%s",
                             major_version, minor_version, bugfix_version);
#elif defined INFINIT_IOS
        return "unknown";
#elif defined INFINIT_ANDROID
        return "unknown"; // FIXME
#else
# error Please define INFINIT_{OS} according to your platform.
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
