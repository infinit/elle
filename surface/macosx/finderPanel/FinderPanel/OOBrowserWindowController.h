#import <Quartz/Quartz.h>
#import <Cocoa/Cocoa.h>
#import "OONetworkBrowserViewManager.h"
#import "OOUserBrowserViewManager.h"

@interface OOBrowserWindowController : NSWindowController {
    IBOutlet IKImageBrowserView*            userBrowser;
    IBOutlet OOUserBrowserViewManager*      userBrowserViewManager;
    
    IBOutlet IKImageBrowserView*            networkBrowser;
    IBOutlet OONetworkBrowserViewManager*   networkBrowserViewManager;
    
    IBOutlet NSTextField*                   networkNameTextField;
    
    BOOL isOpen;
}

@property(nonatomic, assign) BOOL isOpen;

- (id)initWithWindowNib;
@end
