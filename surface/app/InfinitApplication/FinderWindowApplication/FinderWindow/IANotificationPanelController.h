//
//  IANotificationPanelController.h
//  FinderWindowApplication
//
//  Created by infinit on 11/9/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "IANotificationListDelegate.h"

@interface IANotificationPanelController : NSWindowController//<NSWindowDelegate>

- (void)toggleVisibility;
- (void)setVisibility:(BOOL)visibility;
- (BOOL)visibility;

@property (assign) IBOutlet IANotificationListDelegate* source;
@property (assign) IBOutlet NSTableView* table;

@end
