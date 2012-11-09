//
//  IANotificationPanelController.h
//  FinderWindowApplication
//
//  Created by infinit on 11/9/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface IANotificationPanelController : NSWindowController//<NSWindowDelegate>

- (void)toggleVisibility;
- (void)setVisibility:(BOOL)visibility;
- (BOOL)visibility;

@end
