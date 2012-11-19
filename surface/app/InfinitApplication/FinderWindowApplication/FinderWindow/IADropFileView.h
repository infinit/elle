//
//  IADropFileView.h
//  FinderWindowApplication
//
//  Created by infinit on 11/5/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "IAMainViewController.h"

@interface IADropFileView
    : NSView <NSDraggingDestination, NSPasteboardItemDataProvider>

@property(retain) IBOutlet IAMainViewController* main_controller;
@property(retain) IBOutlet NSTextField* label;
@property(readonly) NSArray* pending_files;

- (BOOL) hasPendingFiles;
- (void) reset;

@end
