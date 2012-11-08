//
//  IAAppDelegate.m
//  FinderWindowApplication
//
//  Created by infinit on 10/25/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IAAppDelegate.h"
#import "IAStatusItem.h"
#import <FinderWindow/IAFinderWindowController.h>

@implementation IAAppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{

    self.window_controller = [[IAFinderWindowController alloc] initFromNib];

    NSPoint p;
    p.x = 400;
    p.y = 200;
    [[self.window_controller window] setFrameOrigin:p];
    [self.window_controller showWindow:self];

}
-(void) awakeFromNib
{
    [IAStatusItem load];
}

@end
