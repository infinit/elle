#import <Quartz/Quartz.h>
#import <Foundation/Foundation.h>

@interface OOUserBrowserViewManager : NSObject {
    IBOutlet IKImageBrowserView*    userBrowser;
    NSMutableArray*					users;
    NSMutableArray*					importedUsers;
    NSMutableArray*					filteredUsers;
}

- (void)defineStyle;
- (void)addUsersWithIds:(NSArray*)userIds;
- (void)setFilteredUsers:(NSArray*)userIds;
@end
