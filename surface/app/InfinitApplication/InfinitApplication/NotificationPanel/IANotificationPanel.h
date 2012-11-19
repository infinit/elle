//
//  IANotificationPanel.h
//  FinderWindowApplication
//
//  Created by infinit on 11/9/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface IANotificationPanel : NSPanel

- (void)setStatusBarController:(id)controller;
- (void)show;
- (void)hide;

@end
