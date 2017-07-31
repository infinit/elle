#if defined(ELLE_IOS)
# import <UIKit/UIDevice.h>
#elif defined(ELLE_MACOS)
# include <CoreServices/CoreServices.h>
#endif

#include <elle/printf.hh>
#include <elle/system/platform.hh>

namespace elle
{
  namespace system
  {
    namespace platform
    {
      std::string
      os_name()
      {
#if defined(ELLE_MACOS)
        return "MacOSX";
#elif defined(ELLE_IOS)
        return "iOS";
#endif
      }

      std::string
      os_version()
      {
#if defined(ELLE_MACOS)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wdeprecated-declarations"
        int32_t major_version, minor_version, bugfix_version;
        if (Gestalt(gestaltSystemVersionMajor, &major_version) != noErr)
          return "unknown";
        if (Gestalt(gestaltSystemVersionMinor, &minor_version) != noErr)
          return "unknown";
        if (Gestalt(gestaltSystemVersionBugFix, &bugfix_version) != noErr)
          return "unknown";
        return elle::sprintf("%s.%s.%s",
                             major_version, minor_version, bugfix_version);
# pragma clang diagnostic pop
#elif defined(ELLE_IOS)
        return [UIDevice currentDevice].systemVersion.UTF8String;
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