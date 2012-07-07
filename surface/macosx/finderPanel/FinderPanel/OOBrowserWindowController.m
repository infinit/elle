#import "OOBrowserWindowController.h"
#import "OOUserBrowserBackgroundLayer.h"

//==============================================================================
@implementation OOBrowserWindowController

- (void)awakeFromNib
{
    [userBrowserViewManager defineStyle];
    [networkBrowserViewManager defineStyle];
    NSArray* users = [NSArray arrayWithObjects:@"qsdjskbdhqsuiè7T", @"qsdjsdsdskbdhqsuiè7T", nil];
    if (users)
	{ 
        // launch import in an independent thread
		[NSThread detachNewThreadSelector:@selector(addUsersWithIds:) toTarget:userBrowserViewManager withObject:users];
	}
    
    NSArray* networks = [NSArray arrayWithObjects:@"qsdjskbdhqsuiè7T", @"qsdjsdsdskbdhqsuiè7T", nil];
    if (networks)
	{ 
        // launch import in an independent thread
		[NSThread detachNewThreadSelector:@selector(addUsersWithIds:) toTarget:networkBrowserViewManager withObject:networks];
	}
}

@end
