#import <Quartz/Quartz.h>
#import <Foundation/Foundation.h>

@interface OONetworkBrowserViewManager : NSObject {
    IBOutlet IKImageBrowserView*    networkBrowser;
    NSMutableArray*					networks;
    NSMutableArray*					importedNetworks;
}

- (void)defineStyle;
@end
