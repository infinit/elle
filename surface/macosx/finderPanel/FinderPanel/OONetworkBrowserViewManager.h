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
}

@property BOOL updateNetwork, forceUpdateNetwork;

- (void)defineStyle;
- (void)updateNetworksLoop;
- (void)updateNetworks;
@end
