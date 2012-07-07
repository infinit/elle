#import <Quartz/Quartz.h>
#import <Foundation/Foundation.h>

@interface OOUserBrowserViewManager : NSObject {
    IBOutlet IKImageBrowserView*    userBrowser;
    NSMutableArray*					users;
    NSMutableArray*					importedUsers;
}

- (void)defineStyle;
@end
