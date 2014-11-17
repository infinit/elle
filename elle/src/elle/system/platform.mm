#import <UIKit/UIDevice.h>

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
        return "iOS";
      }

      std::string
      os_version()
      {
        return [[[UIDevice currentDevice] systemVersion] UTF8String];
      }

      std::string
      os_description()
      {
        return elle::sprintf("%s %s", os_name(), os_version());
      }
    }
  }
}