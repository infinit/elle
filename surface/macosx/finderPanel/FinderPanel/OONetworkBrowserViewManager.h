#import <Quartz/Quartz.h>
#import <Foundation/Foundation.h>
#import "OOUserBrowserViewManager.h"

@interface OONetworkBrowserViewManager : NSObject {
    IBOutlet IKImageBrowserView*        networkBrowser;
    IBOutlet IKImageBrowserView*        userBrowser;
    IBOutlet OOUserBrowserViewManager*  userBrowserViewManager;
    NSMutableArray*                     networks;
    NSMutableArray*                     importedNetworks;
    BOOL                                updateNetwork, forceUpdateNetwork;
    NSImage*                            defaultNetworkImage;
}

@property BOOL updateNetwork, forceUpdateNetwork;
@property(retain) NSImage* defaultNetworkImage;

- (void)defineStyle;
- (void)forceUpdateNetworks;
- (void)updateNetworksLoop;
- (void)updateNetworks;
@end
