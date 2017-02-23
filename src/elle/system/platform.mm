#if defined(INFINIT_IOS)
# import <UIKit/UIDevice.h>
#elif defined(INFINIT_MACOSX)
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
#if defined(INFINIT_MACOSX)
        return "MacOSX";
#elif defined(INFINIT_IOS)
        return "iOS";
#endif
      }

      std::string
      os_version()
      {
#if defined(INFINIT_MACOSX)
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
#elif defined(INFINIT_IOS)
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