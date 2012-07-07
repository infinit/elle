#import <Quartz/Quartz.h>
#import <Cocoa/Cocoa.h>
#import "OOUserBrowserViewManager.h"
#import "OONetworkBrowserViewManager.h"

@interface OOBrowserWindowController : NSWindowController {
    IBOutlet IKImageBrowserView*            userBrowser;
    IBOutlet OOUserBrowserViewManager*      userBrowserViewManager;
    
    IBOutlet IKImageBrowserView*            networkBrowser;
    IBOutlet OONetworkBrowserViewManager*   networkBrowserViewManager;
}

@end
