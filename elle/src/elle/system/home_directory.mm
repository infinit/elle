#import <Foundation/Foundation.h>

#include <elle/system/home_directory.hh>

namespace elle
{
  namespace system
  {
    boost::filesystem::path
    home_directory()
    {
      NSString* application_support_path =
        NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory,
                                            NSUserDomainMask,
                                            YES).firstObject;
      return application_support_path.UTF8String;
    }
  }
}
