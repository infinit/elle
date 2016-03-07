#import <Foundation/Foundation.h>

#include <elle/system/user_paths.hh>

namespace elle
{
  namespace system
  {
    boost::filesystem::path
    home_directory()
    {
      @autoreleasepool
      {
#if defined(INFINIT_MACOSX)
        return NSHomeDirectory().UTF8String;
#elif defined(INFINIT_IOS)
        NSString* application_support_dir =
          NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory,
                                              NSUserDomainMask,
                                              YES).firstObject;
        return application_support_dir.UTF8String;
#else
# error Unknown plaform.
#endif
      }
    }

    boost::filesystem::path
    download_directory()
    {
      @autoreleasepool
      {
#if defined(INFINIT_MACOSX)
        NSString* download_dir =
          NSSearchPathForDirectoriesInDomains(NSDownloadsDirectory,
                                              NSUserDomainMask,
                                              YES).firstObject;
#elif defined(INFINIT_IOS)
        NSString* doc_dir =
          NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
                                              NSUserDomainMask,
                                              YES).firstObject;
        NSString* download_dir =
          [doc_dir stringByAppendingPathComponent:@"Downloads"];
#else
# error Unknown plaform.
#endif
        return download_dir.UTF8String;
      }
    }
  }
}

