//
//  IAFinderWindowController.m
//  FinderWindowApplication
//
//  Created by infinit on 10/25/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IAFinderWindowController.h"
#import "IALoginViewController.h"

@interface IAFinderWindowController ()

@property (retain) IBOutlet IALoginViewController* login_view_controller;
@property (retain) IBOutlet NSView* login_view;

@end

@implementation IAFinderWindowController

- (id)init
{
    self = [super initWithWindowNibName:@"FinderWindow"];
    
    if (self) {

    }
    
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    [[self window] setContentView:[self.login_view_controller view]];
}


@end