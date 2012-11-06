//
//  IAFinderWindowController.m
//  FinderWindowApplication
//
//  Created by infinit on 10/25/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IAFinderWindowController.h"
#import "IALoginViewController.h"
#import "IAMainViewController.h"
#import "IARegisterViewController.h"

#import "IAGapState.h"

@interface IAFinderWindowController () {
    BOOL _registering;
}

@property (retain) IBOutlet IALoginViewController* login_view_controller;
@property (retain) IBOutlet IARegisterViewController* register_view_controller;
@property (retain) IBOutlet IAMainViewController* main_view_controller;

-(IBAction)switchToRegisterView:(id)sender;
-(IBAction)switchToLoginView:(id)sender;

@end

@implementation IAFinderWindowController

- (id)initFromNib
{
    self = [super initWithWindowNibName:@"FinderWindow"];
    
    if (self) {
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(_onUserLoggedIn:)
                                                     name:IA_GAP_EVENT_LOGIN_OPERATION
                                                   object:nil];
        _registering = false;
    }
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    [self _updateCurrentView];
}

- (void)_onUserLoggedIn:(NSNotification*)notification
{
    if ([[notification name] isEqualToString:IA_GAP_EVENT_LOGIN_OPERATION] &&
        [[notification object] success])
    {
        // We actually are connected!
        [self _updateCurrentView];
    }
}

-(IBAction)switchToRegisterView:(id)sender
{
    _registering = true;
    [self _updateCurrentView];
}

-(IBAction)switchToLoginView:(id)sender
{
    _registering = false;
    [self _updateCurrentView];
}

- (void)_updateCurrentView
{
    NSLog(@"Updating current view");
    if ([IAGapState instance].logged_in)
    {
        [[self window] setContentView:[self.main_view_controller view]];
    }
    else if (_registering)
    {
        [[self window] setContentView:[self.register_view_controller view]];
    }
    else
    {
        [[self window] setContentView:[self.login_view_controller view]];
    }
}

@end
