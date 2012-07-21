#import "OOBrowserWindowController.h"
#import "OOUserBrowserBackgroundLayer.h"
#import <Phone/OOPhone.h>

//==============================================================================
@implementation OOBrowserWindowController

@synthesize isOpen;

- (id)initWithWindowNib {
    return [self initWithWindowNibName:@"OOBrowserWindow"];
}

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
		[NSThread detachNewThreadSelector:@selector(updateNetworks) toTarget:networkBrowserViewManager withObject:nil];
	}
}

- (IBAction)startAnimations:(id)sender
{
    if (self.isOpen) {
        [self hideView];
        self.isOpen = NO;
    } else {
        [self showView];
        self.isOpen = YES;
    }
}
- (void)hideView {
    // firstView, secondView are outlets
    NSViewAnimation *theAnim;
    NSRect firstViewFrame;
    NSRect newViewFrame;
    NSMutableDictionary* firstViewDict;
    {
        // Create the attributes dictionary for the first view.
        firstViewDict = [NSMutableDictionary dictionaryWithCapacity:3];
        firstViewFrame = [self.window frame];
        
        // Specify which view to modify.
        [firstViewDict setObject:self.window forKey:NSViewAnimationTargetKey];
        
        // Specify the starting position of the view.
        [firstViewDict setObject:[NSValue valueWithRect:firstViewFrame]
                          forKey:NSViewAnimationStartFrameKey];
        
        // Change the ending position of the view.
        newViewFrame = firstViewFrame;
        newViewFrame.origin.y = 30 - [self.window frame].size.height;
        [firstViewDict setObject:[NSValue valueWithRect:newViewFrame]
                          forKey:NSViewAnimationEndFrameKey];
    }    // Create the view animation object.
    theAnim = [[NSViewAnimation alloc] initWithViewAnimations:[NSArray
                                                               arrayWithObjects:firstViewDict, nil]];
    
    // Set some additional attributes for the animation.
    [theAnim setDuration:0.5];    // One and a half seconds.
    [theAnim setAnimationCurve:NSAnimationEaseInOut];
    
    // Run the animation.
    [theAnim startAnimation];
}
- (void)showView{
    // firstView, secondView are outlets
    NSViewAnimation *theAnim;
    NSRect firstViewFrame;
    NSRect newViewFrame;
    NSMutableDictionary* firstViewDict;
    {
        // Create the attributes dictionary for the first view.
        firstViewDict = [NSMutableDictionary dictionaryWithCapacity:3];
        firstViewFrame = [self.window frame];
        
        // Specify which view to modify.
        [firstViewDict setObject:self.window forKey:NSViewAnimationTargetKey];
        
        // Specify the starting position of the view.
        [firstViewDict setObject:[NSValue valueWithRect:firstViewFrame]
                          forKey:NSViewAnimationStartFrameKey];
        
        // Change the ending position of the view.
        newViewFrame = firstViewFrame;
        newViewFrame.origin.y = 0;
        [firstViewDict setObject:[NSValue valueWithRect:newViewFrame]
                          forKey:NSViewAnimationEndFrameKey];
    }    // Create the view animation object.
    theAnim = [[NSViewAnimation alloc] initWithViewAnimations:[NSArray
                                                               arrayWithObjects:firstViewDict, nil]];
    
    // Set some additional attributes for the animation.
    [theAnim setDuration:0.5];    // One and a half seconds.
    [theAnim setAnimationCurve:NSAnimationEaseInOut];
    
    // Run the animation.
    [theAnim startAnimation];
    
    
}


// -------------------------------------------------------------------------
//	performDragOperation:sender
// ------------------------------------------------------------------------- 
- (IBAction)createNetwork:(id)sender {
    [[OOPhone getInstance] createNetworkWithName:@"Hello"];
}

@end
