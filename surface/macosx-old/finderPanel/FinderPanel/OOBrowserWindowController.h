#import <Quartz/Quartz.h>
#import <Cocoa/Cocoa.h>
#import "OONetworkBrowserViewManager.h"
#import "OOUserBrowserViewManager.h"

extern NSString *OOBrowserWindowControllerSetOffView;
extern NSString *OOBrowserWindowControllerSetOnView;

@interface OOBrowserWindowController : NSWindowController {
    IBOutlet NSView*                        onView;
    IBOutlet NSView*                        offView;
    
    IBOutlet IKImageBrowserView*            userBrowser;
    IBOutlet OOUserBrowserViewManager*      userBrowserViewManager;
    
    IBOutlet IKImageBrowserView*            networkBrowser;
    IBOutlet OONetworkBrowserViewManager*   networkBrowserViewManager;
    
    IBOutlet NSTextField*                   networkNameTextField;
    
    BOOL                                    isOpen;
}

@property(nonatomic, assign)    BOOL                    isOpen;
@property(retain)               IBOutlet NSView*        onView;
@property(retain)               IBOutlet NSView*        offView;

- (id) initWithWindowNib;
- (void) switchToOn;
- (void) switchToOff;

@end
