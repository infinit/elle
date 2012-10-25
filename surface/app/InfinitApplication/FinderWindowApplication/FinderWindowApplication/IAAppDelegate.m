//
//  IAAppDelegate.m
//  FinderWindowApplication
//
//  Created by infinit on 10/25/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IAAppDelegate.h"
#import <FinderWindow/IAFinderWindowController.h>

@implementation IAAppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    self.window_controller = [[IAFinderWindowController alloc] init];
    [self.window_controller showWindow:self];
}

@end
