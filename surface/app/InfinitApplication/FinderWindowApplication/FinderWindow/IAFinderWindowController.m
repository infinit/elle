//
//  IAFinderWindowController.m
//  FinderWindowApplication
//
//  Created by infinit on 10/25/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IAFinderWindowController.h"
#import "IALoginViewController.h"
#import "IAGapState.h"

@interface IAFinderWindowController ()

@property (retain) IBOutlet IALoginViewController* login_view_controller;
//@property (retain) IBOutlet NSView* login_view;

@end

@implementation IAFinderWindowController

- (id)init
{
    self = [super initWithWindowNibName:@"FinderWindow"];
    
    if (self) {
        NSLog(@"SUBSCRIBE for %@", self);
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(_onUserLoggedIn:)
                                                     name:IA_GAP_EVENT_LOGIN_OPERATION
                                                   object:nil];
    }
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    [[self window] setContentView:[self.login_view_controller view]];
}

- (void)_onUserLoggedIn:(NSNotification*)notification
{
    if ([[notification name] isEqualToString:IA_GAP_EVENT_LOGIN_OPERATION])
    {
        IAGapOperationResult* result = [notification object];
        NSLog(@"Yeah, got user notification: %@", result);
    }
}

@end